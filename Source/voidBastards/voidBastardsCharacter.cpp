// Copyright Epic Games, Inc. All Rights Reserved.

#include "voidBastardsCharacter.h"
#include "voidBastardsProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "weapons/Regulator.h"
#include "weapons/Device.h"
#include "weapons/BushWhacker.h"
#include "damagable.h"
#include "Part.h"
#include "ExitDoor.h"
#include "MyCharacterMovementComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "GameFramework/CharacterMovementComponent.h"
#include "VisualCollicion.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AvoidBastardsCharacter

AvoidBastardsCharacter::AvoidBastardsCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	//GetMovementComponent

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	firearm = CreateDefaultSubobject<URegulator>("fire arm");
	device = CreateDefaultSubobject<UDevice>("device");
	indirect = CreateDefaultSubobject<UBushWhacker>("indirect");
	actualWeapon = firearm;
	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;

	damagable = CreateDefaultSubobject<Udamagable>(TEXT("damagable"));

	OnDestroyed.AddDynamic(this, &AvoidBastardsCharacter::MyDestroyed);
}

void AvoidBastardsCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	oxigen = 60;
	maxOxigen = 60;
	//size_t id = (size_t)GetMovementComponent();
	//
	//size_t* scaner = (size_t*)this;
	//
	//while(true){
  //  if(id==*scaner){
  //    break;
  //  }
  //  ++scaner;
  //}
	//auto comp = new UMyCharacterMovementComponent;
  //*scaner = (size_t)();

	//firearm->character = this;
	//
	//device->character = this;
	//
	//indirect->character = this;

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}
}

void 
AvoidBastardsCharacter::Tick(float DeltaTime)
{
	update();
	
		///GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%f"),  FVector::Distance(GetActorLocation(),result.ImpactPoint)));

	
	oxigen-=DeltaTime;
	if(health > damagable->getVidaPercentage()){
		health -= DeltaTime*.125f;
	}

	if(oxigen<0){
		die();
	}

	if(shooting && actualWeapon){
		actualWeapon->delta = DeltaTime;
		actualWeapon->shotting();
		
	}

	if(isDead){
		deadTimer += DeltaTime;
		if(deadTimer>1){
			UGameplayStatics::OpenLevel(GetWorld(),"workbench");
		}
	}
	
	//GetMovementComponent()->Velocity =  FVector::ZeroVector;
	//CharacterMovement;

	
	//if(upVel<0){
	//	upVel = 0;
	//}
	

	


	

	//auto castComp = Cast<UMyCharacterMovementComponent>(comp);
	//
	//if(castComp)
	
	
	
}


float AvoidBastardsCharacter::getOxigenPercentage()
{
	return oxigen/maxOxigen;
}

//////////////////////////////////////////////////////////////////////////
// Input

void 
AvoidBastardsCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AvoidBastardsCharacter::OnFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AvoidBastardsCharacter::OnEndFire);

	PlayerInputComponent->BindAction("Use", IE_Pressed, this, &AvoidBastardsCharacter::use);
	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AvoidBastardsCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AvoidBastardsCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AvoidBastardsCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AvoidBastardsCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AvoidBastardsCharacter::LookUpAtRate);
}

void AvoidBastardsCharacter::OnFire()
{
	if(actualWeapon)
	actualWeapon->use();

	shooting = true;

	

	// try and fire a projectile
	/*
	if (ProjectileClass != nullptrUMyUserWidget
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<AvoidBastardsProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<AvoidBastardsProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}
	}

	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
	*/
}

void AvoidBastardsCharacter::OnEndFire()
{
	if(actualWeapon)
	actualWeapon->stopShotting();

	shooting = false;
}

void AvoidBastardsCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AvoidBastardsCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AvoidBastardsCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void AvoidBastardsCharacter::use()
{
	auto dir = UGameplayStatics::GetPlayerCameraManager(GetWorld(),0)->GetActorForwardVector();
	auto start = UGameplayStatics::GetPlayerCameraManager(GetWorld(),0)->GetCameraLocation();
  auto end = start + dir*1000.f;
  FHitResult result;
  GetWorld()->LineTraceSingleByChannel(result,start,end,ECollisionChannel::ECC_Visibility);
	DrawDebugLine(GetWorld(), start, result.ImpactPoint, FColor::Emerald, true, -1, 0, 10);

	onUse(result);
	//auto actor = Cast<AExitDoor>(result.GetActor());
	//if(actor){
	//	exitLevel();
	//	UGameplayStatics::OpenLevel(GetWorld(),"workbench");
	//}
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AvoidBastardsCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void AvoidBastardsCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		//auto col = Cast<UVisualCollicion>(GetComponentByClass(UVisualCollicion::StaticClass()));
		//float dotCollicion = FVector::DotProduct(col->collidingVector,GetActorForwardVector()*Value);
		//
		//if(!col->isColliding || dotCollicion <=0)
		//AddActorWorldOffset(GetActorForwardVector()*Value*velocity,true);
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AvoidBastardsCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		//auto col = Cast<UVisualCollicion>(GetComponentByClass(UVisualCollicion::StaticClass()));
		//float dotCollicion = FVector::DotProduct(col->collidingVector,GetActorRightVector()*Value);
		//if(!col->isColliding  || dotCollicion<=0)
		//AddActorWorldOffset(GetActorRightVector()*Value*velocity,true);
		AddMovementInput(GetActorRightVector(), Value);
	}
}


void AvoidBastardsCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AvoidBastardsCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AvoidBastardsCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AvoidBastardsCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AvoidBastardsCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AvoidBastardsCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}

void 
AvoidBastardsCharacter::MyDestroyed(AActor* Act)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Destoyed"));
}

void AvoidBastardsCharacter::attachWeapon(UWeapon* weapon)
{
	//if(!weapon){
	//	return;
	//}
	//
	//weapon->character = this;
}
