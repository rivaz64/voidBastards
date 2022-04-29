// Fill out your copyright notice in the Description page of Project Settings.
#include "PlayerCharacter.h"

#include "GameFramework/DamageType.h"
#include "GameFramework/Controller.h"
#include "Components/SkinnedMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Engine/CollisionProfile.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "DisplayDebugHelpers.h"
#include "Engine/Canvas.h"
#include "Animation/AnimInstance.h"
#include "MyCharacterMovementComponent.h"
#include "damagable.h"
#include "weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"

FName APlayerCharacter::MeshComponentName="Mesh";
FName APlayerCharacter::CapsuleComponentName="Capsule";

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
//: Super(ObjectInitializer)
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		FName ID_Characters;
		FText NAME_Characters;
		FConstructorStatics()
			: ID_Characters(TEXT("Characters"))
			, NAME_Characters(NSLOCTEXT("SpriteCategory", "Characters", "Characters"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Character rotation only changes in Yaw, to prevent the capsule from changing orientation.
	// Ask the Controller for the full rotation if desired (ie for aiming).
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(APlayerCharacter::CapsuleComponentName);
	CapsuleComponent->InitCapsuleSize(34.0f, 88.0f);
	CapsuleComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

	CapsuleComponent->CanCharacterStepUpOn = ECB_No;
	CapsuleComponent->SetShouldUpdatePhysicsVolume(true);
	CapsuleComponent->SetCanEverAffectNavigation(false);
	CapsuleComponent->bDynamicObstacle = true;
	RootComponent = CapsuleComponent;

	bClientCheckEncroachmentOnNetUpdate = true;
	JumpKeyHoldTime = 0.0f;
	JumpMaxHoldTime = 0.0f;
    JumpMaxCount = 1;
    JumpCurrentCount = 0;
	JumpCurrentCountPreJump = 0;
    bWasJumping = false;

	AnimRootMotionTranslationScale = 1.0f;

#if WITH_EDITORONLY_DATA
	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	if (ArrowComponent)
	{
		ArrowComponent->ArrowColor = FColor(150, 200, 255);
		ArrowComponent->bTreatAsASprite = true;
		ArrowComponent->SpriteInfo.Category = ConstructorStatics.ID_Characters;
		ArrowComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_Characters;
		ArrowComponent->SetupAttachment(CapsuleComponent);
		ArrowComponent->bIsScreenSizeScaled = true;
	}
#endif // WITH_EDITORONLY_DATA

	CharacterMovement = CreateDefaultSubobject<UMyCharacterMovementComponent>(TEXT("CharMoveComp"));
	if (CharacterMovement)
	{
		CharacterMovement->UpdatedComponent = CapsuleComponent;
		CrouchedEyeHeight = CharacterMovement->CrouchedHalfHeight * 0.80f;
	}

	Mesh = CreateOptionalDefaultSubobject<USkeletalMeshComponent>(APlayerCharacter::MeshComponentName);
	if (Mesh)
	{
		Mesh->AlwaysLoadOnClient = true;
		Mesh->AlwaysLoadOnServer = true;
		Mesh->bOwnerNoSee = false;
		Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
		Mesh->bCastDynamicShadow = true;
		Mesh->bAffectDynamicIndirectLighting = true;
		Mesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		Mesh->SetupAttachment(CapsuleComponent);
		static FName MeshCollisionProfileName(TEXT("CharacterMesh"));
		Mesh->SetCollisionProfileName(MeshCollisionProfileName);
		Mesh->SetGenerateOverlapEvents(false);
		Mesh->SetCanEverAffectNavigation(false);
	}

	BaseRotationOffset = FQuat::Identity;
}

void APlayerCharacter::PostInitializeComponents()
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_Character_PostInitComponents);

	Super::PostInitializeComponents();

	if (!IsPendingKill())
	{
		if (Mesh)
		{
			CacheInitialMeshOffset(Mesh->GetRelativeLocation(), Mesh->GetRelativeRotation());

			// force animation tick after movement component updates
			if (Mesh->PrimaryComponentTick.bCanEverTick && CharacterMovement)
			{
				Mesh->PrimaryComponentTick.AddPrerequisite(CharacterMovement, CharacterMovement->PrimaryComponentTick);
			}
		}

		if (CharacterMovement && CapsuleComponent)
		{
			CharacterMovement->UpdateNavAgent(*CapsuleComponent);
		}

		if (Controller == nullptr && GetNetMode() != NM_Client)
		{
			if (CharacterMovement && CharacterMovement->bRunPhysicsWithNoController)
			{
				CharacterMovement->SetDefaultMovementMode();
			}
		}
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}


void APlayerCharacter::CacheInitialMeshOffset(FVector MeshRelativeLocation, FRotator MeshRelativeRotation)
{
	BaseTranslationOffset = MeshRelativeLocation;
	BaseRotationOffset = MeshRelativeRotation.Quaternion();

#if ENABLE_NAN_DIAGNOSTIC
	if (BaseRotationOffset.ContainsNaN())
	{
		logOrEnsureNanError(TEXT("APlayerCharacter::PostInitializeComponents detected NaN in BaseRotationOffset! (%s)"), *BaseRotationOffset.ToString());
	}

	const FRotator LocalRotation = Mesh->GetRelativeRotation();
	if (LocalRotation.ContainsNaN())
	{
		logOrEnsureNanError(TEXT("APlayerCharacter::PostInitializeComponents detected NaN in Mesh->RelativeRotation! (%s)"), *LocalRotation.ToString());
	}
#endif
}


UPawnMovementComponent* APlayerCharacter::GetMovementComponent() const
{
	return CharacterMovement;
}


void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
}


void APlayerCharacter::GetSimpleCollisionCylinder(float& CollisionRadius, float& CollisionHalfHeight) const
{
//#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
//	if (IsTemplate())
//	{
//		UE_LOG(LogCharacter, Log, TEXT("WARNING APlayerCharacter::GetSimpleCollisionCylinder : Called on default object '%s'. Will likely return zero size. Consider using GetDefaultHalfHeight() instead."), *this->GetPathName());
//	}
//#endif

	if (RootComponent == CapsuleComponent && IsRootComponentCollisionRegistered())
	{
		// Note: we purposefully ignore the component transform here aside from scale, always treating it as vertically aligned.
		// This improves performance and is also how we stated the CapsuleComponent would be used.
		CapsuleComponent->GetScaledCapsuleSize(CollisionRadius, CollisionHalfHeight);
	}
	else
	{
		Super::GetSimpleCollisionCylinder(CollisionRadius, CollisionHalfHeight);
	}
}

void APlayerCharacter::UpdateNavigationRelevance()
{
	if (CapsuleComponent)
	{
		CapsuleComponent->SetCanEverAffectNavigation(bCanAffectNavigationGeneration);
	}
}

float APlayerCharacter::GetDefaultHalfHeight() const
{
	UCapsuleComponent* DefaultCapsule = GetClass()->GetDefaultObject<APlayerCharacter>()->CapsuleComponent;
	if (DefaultCapsule)
	{
		return DefaultCapsule->GetScaledCapsuleHalfHeight();
	}
	else
	{
		return 0.0f;//Super::GetDefaultHalfHeight();
	}
}


UActorComponent* APlayerCharacter::FindComponentByClass(const TSubclassOf<UActorComponent> ComponentClass) const
{
	// If the character has a Mesh, treat it as the first 'hit' when finding components
	if (Mesh && ComponentClass && Mesh->IsA(ComponentClass))
	{
		return Mesh;
	}

	return Super::FindComponentByClass(ComponentClass);
}

void APlayerCharacter::OnWalkingOffLedge_Implementation(const FVector& PreviousFloorImpactNormal, const FVector& PreviousFloorContactNormal, const FVector& PreviousLocation, float TimeDelta)
{
}

void APlayerCharacter::NotifyJumpApex()
{
	// Call delegate callback
	if (OnReachedJumpApex.IsBound())
	{
		OnReachedJumpApex.Broadcast();
	}
}

void APlayerCharacter::Landed(const FHitResult& Hit)
{
	OnLanded(Hit);

	LandedDelegate.Broadcast(Hit);
}

bool APlayerCharacter::CanJump() const
{
	return CanJumpInternal();
}

bool APlayerCharacter::CanJumpInternal_Implementation() const
{
	// Ensure the character isn't currently crouched.
	bool bCanJump = !bIsCrouched;

	// Ensure that the CharacterMovement state is valid
	bCanJump &= CharacterMovement->CanAttemptJump();

	if (bCanJump)
	{
		// Ensure JumpHoldTime and JumpCount are valid.
		if (!bWasJumping || GetJumpMaxHoldTime() <= 0.0f)
		{
			if (JumpCurrentCount == 0 && CharacterMovement->IsFalling())
			{
				bCanJump = JumpCurrentCount + 1 < JumpMaxCount;
			}
			else
			{
				bCanJump = JumpCurrentCount < JumpMaxCount;
			}
		}
		else
		{
			// Only consider JumpKeyHoldTime as long as:
			// A) The jump limit hasn't been met OR
			// B) The jump limit has been met AND we were already jumping
			const bool bJumpKeyHeld = (bPressedJump && JumpKeyHoldTime < GetJumpMaxHoldTime());
			bCanJump = bJumpKeyHeld &&
						((JumpCurrentCount < JumpMaxCount) || (bWasJumping && JumpCurrentCount == JumpMaxCount));
		}
	}

	return bCanJump;
}

void APlayerCharacter::ResetJumpState()
{
	bPressedJump = false;
	bWasJumping = false;
	JumpKeyHoldTime = 0.0f;
	JumpForceTimeRemaining = 0.0f;

	if (CharacterMovement && !CharacterMovement->IsFalling())
	{
		JumpCurrentCount = 0;
		JumpCurrentCountPreJump = 0;
	}
}

void APlayerCharacter::OnJumped_Implementation()
{
}

bool APlayerCharacter::IsJumpProvidingForce() const
{
	if (JumpForceTimeRemaining > 0.0f)
	{
		return true;
	}
	else if (bProxyIsJumpForceApplied && (GetLocalRole() == ROLE_SimulatedProxy))
	{
		return GetWorld()->TimeSince(ProxyJumpForceStartedTime) <= GetJumpMaxHoldTime();
	}

	return false;
}

void APlayerCharacter::RecalculateBaseEyeHeight()
{
	if (!bIsCrouched)
	{
		//Super::RecalculateBaseEyeHeight();
	}
	else
	{
		BaseEyeHeight = CrouchedEyeHeight;
	}
}


void APlayerCharacter::OnRep_IsCrouched()
{
	if (CharacterMovement)
	{
		if (bIsCrouched)
		{
			CharacterMovement->bWantsToCrouch = true;
			CharacterMovement->Crouch(true);
		}
		else
		{
			CharacterMovement->bWantsToCrouch = false;
			CharacterMovement->UnCrouch(true);
		}
		CharacterMovement->bNetworkUpdateReceived = true;
	}
}

void APlayerCharacter::SetReplicateMovement(bool bInReplicateMovement)
{
	Super::SetReplicateMovement(bInReplicateMovement);

	if (CharacterMovement != nullptr && GetLocalRole() == ROLE_Authority)
	{
		// Set prediction data time stamp to current time to stop extrapolating
		// from time bReplicateMovement was turned off to when it was turned on again
		FNetworkPredictionData_Server* NetworkPrediction = CharacterMovement->HasPredictionData_Server() ? CharacterMovement->GetPredictionData_Server() : nullptr;

		if (NetworkPrediction != nullptr)
		{
			NetworkPrediction->ServerTimeStamp = GetWorld()->GetTimeSeconds();
		}
	}
}

bool APlayerCharacter::CanCrouch() const
{
	return !bIsCrouched && CharacterMovement && CharacterMovement->CanEverCrouch() && GetRootComponent() && !GetRootComponent()->IsSimulatingPhysics();
}

void APlayerCharacter::Crouch(bool bClientSimulation)
{
	if (CharacterMovement)
	{
		if (CanCrouch())
		{
			CharacterMovement->bWantsToCrouch = true;
		}
//#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
//		else if (!CharacterMovement->CanEverCrouch())
//		{
//			UE_LOG(LogCharacter, Log, TEXT("%s is trying to crouch, but crouching is disabled on this character! (check CharacterMovement NavAgentSettings)"), *GetName());
//		}
//#endif
	}
}

void APlayerCharacter::UnCrouch(bool bClientSimulation)
{
	if (CharacterMovement)
	{
		CharacterMovement->bWantsToCrouch = false;
	}
}


void APlayerCharacter::OnEndCrouch( float HeightAdjust, float ScaledHeightAdjust )
{
	RecalculateBaseEyeHeight();

	const APlayerCharacter* DefaultChar = GetDefault<APlayerCharacter>(GetClass());
	if (Mesh && DefaultChar->Mesh)
	{
		FVector& MeshRelativeLocation = Mesh->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->Mesh->GetRelativeLocation().Z;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		BaseTranslationOffset.Z = DefaultChar->BaseTranslationOffset.Z;
	}

	K2_OnEndCrouch(HeightAdjust, ScaledHeightAdjust);
}

void APlayerCharacter::OnStartCrouch( float HeightAdjust, float ScaledHeightAdjust )
{
	RecalculateBaseEyeHeight();

	const APlayerCharacter* DefaultChar = GetDefault<APlayerCharacter>(GetClass());
	if (Mesh && DefaultChar->Mesh)
	{
		FVector& MeshRelativeLocation = Mesh->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->Mesh->GetRelativeLocation().Z + HeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		BaseTranslationOffset.Z = DefaultChar->BaseTranslationOffset.Z + HeightAdjust;
	}

	K2_OnStartCrouch(HeightAdjust, ScaledHeightAdjust);
}

void APlayerCharacter::ApplyDamageMomentum(float DamageTaken, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser)
{
	UDamageType const* const DmgTypeCDO = DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>();
	float const ImpulseScale = DmgTypeCDO->DamageImpulse;

	if ( (ImpulseScale > 3.f) && (CharacterMovement != nullptr) )
	{
		FHitResult HitInfo;
		FVector ImpulseDir;
		DamageEvent.GetBestHitInfo(this, PawnInstigator, HitInfo, ImpulseDir);

		FVector Impulse = ImpulseDir * ImpulseScale;
		bool const bMassIndependentImpulse = !DmgTypeCDO->bScaleMomentumByMass;

		// limit Z momentum added if already going up faster than jump (to avoid blowing character way up into the sky)
		{
			FVector MassScaledImpulse = Impulse;
			if(!bMassIndependentImpulse && CharacterMovement->Mass > SMALL_NUMBER)
			{
				MassScaledImpulse = MassScaledImpulse / CharacterMovement->Mass;
			}

			if ( (CharacterMovement->Velocity.Z > GetDefault<UCharacterMovementComponent>(CharacterMovement->GetClass())->JumpZVelocity) && (MassScaledImpulse.Z > 0.f) )
			{
				Impulse.Z *= 0.5f;
			}
		}

		CharacterMovement->AddImpulse(Impulse, bMassIndependentImpulse);
	}
}

void APlayerCharacter::ClearCrossLevelReferences()
{
	if( BasedMovement.MovementBase != nullptr && GetOutermost() != BasedMovement.MovementBase->GetOutermost() )
	{
		SetBase( nullptr );
	}

	Super::ClearCrossLevelReferences();
}


/**	Change the Pawn's base. */
void APlayerCharacter::SetBase( UPrimitiveComponent* NewBaseComponent, const FName InBoneName, bool bNotifyPawn )
{
	// If NewBaseComponent is nullptr, ignore bone name.
	const FName BoneName = (NewBaseComponent ? InBoneName : NAME_None);

	// See what changed.
	const bool bBaseChanged = (NewBaseComponent != BasedMovement.MovementBase);
	const bool bBoneChanged = (BoneName != BasedMovement.BoneName);

	if (bBaseChanged || bBoneChanged)
	{
		// Verify no recursion.
		APawn* Loop = (NewBaseComponent ? Cast<APawn>(NewBaseComponent->GetOwner()) : nullptr);
		while (Loop)
		{
			if (Loop == this)
			{
				//UE_LOG(LogCharacter, Warning, TEXT(" SetBase failed! Recursion detected. Pawn %s already based on %s."), *GetName(), *NewBaseComponent->GetName()); //-V595
				return;
			}
			if (UPrimitiveComponent* LoopBase =	Loop->GetMovementBase())
			{
				Loop = Cast<APawn>(LoopBase->GetOwner());
			}
			else
			{
				break;
			}
		}

		// Set base.
		UPrimitiveComponent* OldBase = BasedMovement.MovementBase;
		BasedMovement.MovementBase = NewBaseComponent;
		BasedMovement.BoneName = BoneName;

		if (CharacterMovement)
		{
			const bool bBaseIsSimulating = MovementBaseUtility::IsSimulatedBase(NewBaseComponent);
			if (bBaseChanged)
			{
				MovementBaseUtility::RemoveTickDependency(CharacterMovement->PrimaryComponentTick, OldBase);
				// We use a special post physics function if simulating, otherwise add normal tick prereqs.
				if (!bBaseIsSimulating)
				{
					MovementBaseUtility::AddTickDependency(CharacterMovement->PrimaryComponentTick, NewBaseComponent);
				}
			}

			if (NewBaseComponent)
			{
				// Update OldBaseLocation/Rotation as those were referring to a different base
				// ... but not when handling replication for proxies (since they are going to copy this data from the replicated values anyway)
				if (!bInBaseReplication)
				{
					// Force base location and relative position to be computed since we have a new base or bone so the old relative offset is meaningless.
					CharacterMovement->SaveBaseLocation();
				}

				// Enable PostPhysics tick if we are standing on a physics object, as we need to to use post-physics transforms
				CharacterMovement->PostPhysicsTickFunction.SetTickFunctionEnable(bBaseIsSimulating);
			}
			else
			{
				BasedMovement.BoneName = NAME_None; // None, regardless of whether user tried to set a bone name, since we have no base component.
				BasedMovement.bRelativeRotation = false;
				CharacterMovement->CurrentFloor.Clear();
				CharacterMovement->PostPhysicsTickFunction.SetTickFunctionEnable(false);
			}

			const ENetRole LocalRole = GetLocalRole();
			if (LocalRole == ROLE_Authority || LocalRole == ROLE_AutonomousProxy)
			{
				BasedMovement.bServerHasBaseComponent = (BasedMovement.MovementBase != nullptr); // Also set on proxies for nicer debugging.
				//UE_LOG(LogCharacter, Verbose, TEXT("Setting base on %s for '%s' to '%s'"), LocalRole == ROLE_Authority ? TEXT("Server") : TEXT("AutoProxy"), *GetName(), *GetFullNameSafe(NewBaseComponent));
			}
			else
			{
				//UE_LOG(LogCharacter, Verbose, TEXT("Setting base on Client for '%s' to '%s'"), *GetName(), *GetFullNameSafe(NewBaseComponent));
			}

		}

		// Notify this actor of his new floor.
		if ( bNotifyPawn )
		{
			BaseChange();
		}
	}
}


void APlayerCharacter::SaveRelativeBasedMovement(const FVector& NewRelativeLocation, const FRotator& NewRotation, bool bRelativeRotation)
{
	checkSlow(BasedMovement.HasRelativeLocation());
	BasedMovement.Location = NewRelativeLocation;
	BasedMovement.Rotation = NewRotation;
	BasedMovement.bRelativeRotation = bRelativeRotation;
}

FVector APlayerCharacter::GetNavAgentLocation() const
{
	FVector AgentLocation = FNavigationSystem::InvalidLocation;

	if (GetCharacterMovement() != nullptr)
	{
		AgentLocation = GetCharacterMovement()->GetActorFeetLocation();
	}

	if (FNavigationSystem::IsValidLocation(AgentLocation) == false && CapsuleComponent != nullptr)
	{
		AgentLocation = GetActorLocation() - FVector(0, 0, CapsuleComponent->GetScaledCapsuleHalfHeight());
	}

	return AgentLocation;
}

void APlayerCharacter::TurnOff()
{
	if (CharacterMovement != nullptr)
	{
		CharacterMovement->StopMovementImmediately();
		CharacterMovement->DisableMovement();
	}

	if (GetNetMode() != NM_DedicatedServer && Mesh != nullptr)
	{
		Mesh->bPauseAnims = true;
		if (Mesh->IsSimulatingPhysics())
		{
			Mesh->bBlendPhysics = true;
			Mesh->KinematicBonesUpdateType = EKinematicBonesUpdateToPhysics::SkipAllBones;
		}
	}

	//Super::TurnOff();
}

void APlayerCharacter::Restart()
{
	//Super::Restart();

    JumpCurrentCount = 0;
	JumpCurrentCountPreJump = 0;

	bPressedJump = false;
	ResetJumpState();
	UnCrouch(true);

	if (CharacterMovement)
	{
		CharacterMovement->SetDefaultMovementMode();
	}
}

void APlayerCharacter::PawnClientRestart()
{
	if (CharacterMovement != nullptr)
	{
		CharacterMovement->StopMovementImmediately();
		CharacterMovement->ResetPredictionData_Client();
	}

	//Super::PawnClientRestart();
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	//Super::PossessedBy(NewController);

	// If we are controlled remotely, set animation timing to be driven by client's network updates. So timing and events remain in sync.
	if (Mesh && IsReplicatingMovement() && (GetRemoteRole() == ROLE_AutonomousProxy && GetNetConnection() != nullptr))
	{
		Mesh->bOnlyAllowAutonomousTickPose = true;
	}
}

void APlayerCharacter::UnPossessed()
{
	//Super::UnPossessed();

	if (CharacterMovement)
	{
		CharacterMovement->ResetPredictionData_Client();
		CharacterMovement->ResetPredictionData_Server();
	}

	// We're no longer controlled remotely, resume regular ticking of animations.
	if (Mesh)
	{
		Mesh->bOnlyAllowAutonomousTickPose = false;
	}
}


void APlayerCharacter::TornOff()
{
	Super::TornOff();

	if (CharacterMovement)
	{
		CharacterMovement->ResetPredictionData_Client();
		CharacterMovement->ResetPredictionData_Server();
	}

	// We're no longer controlled remotely, resume regular ticking of animations.
	if (Mesh)
	{
		Mesh->bOnlyAllowAutonomousTickPose = false;
	}
}


void APlayerCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	NumActorOverlapEventsCounter++;
	Super::NotifyActorBeginOverlap(OtherActor);
}

void APlayerCharacter::NotifyActorEndOverlap(AActor* OtherActor)
{
	NumActorOverlapEventsCounter++;
	Super::NotifyActorEndOverlap(OtherActor);
}

void APlayerCharacter::BaseChange()
{
	if (CharacterMovement && CharacterMovement->MovementMode != MOVE_None)
	{
		AActor* ActualMovementBase = GetMovementBaseActor(this);
		if ((ActualMovementBase != nullptr) && !ActualMovementBase->CanBeBaseForCharacter(this))
		{
			CharacterMovement->JumpOff(ActualMovementBase);
		}
	}
}

void APlayerCharacter::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	float Indent = 0.f;

	static FName NAME_Physics = FName(TEXT("Physics"));
	if (DebugDisplay.IsDisplayOn(NAME_Physics) )
	{
		FIndenter PhysicsIndent(Indent);

		FString BaseString;
		if ( CharacterMovement == nullptr || BasedMovement.MovementBase == nullptr )
		{
			BaseString = "Not Based";
		}
		else
		{
			BaseString = BasedMovement.MovementBase->IsWorldGeometry() ? "World Geometry" : BasedMovement.MovementBase->GetName();
			BaseString = FString::Printf(TEXT("Based On %s"), *BaseString);
		}
		
		FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;
		DisplayDebugManager.DrawString(FString::Printf(TEXT("RelativeLoc: %s Rot: %s %s"), *BasedMovement.Location.ToCompactString(), *BasedMovement.Rotation.ToCompactString(), *BaseString), Indent);

		if ( CharacterMovement != nullptr )
		{
			CharacterMovement->DisplayDebug(Canvas, DebugDisplay, YL, YPos);
		}
		const bool Crouched = CharacterMovement && CharacterMovement->IsCrouching();
		FString T = FString::Printf(TEXT("Crouched %i"), Crouched);
		DisplayDebugManager.DrawString(T, Indent);
	}
}

void APlayerCharacter::LaunchCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride)
{
	//UE_LOG(LogCharacter, Verbose, TEXT("APlayerCharacter::LaunchCharacter '%s' (%f,%f,%f)"), *GetName(), LaunchVelocity.X, LaunchVelocity.Y, LaunchVelocity.Z);

	if (CharacterMovement)
	{
		FVector FinalVel = LaunchVelocity;
		const FVector Velocity = GetVelocity();

		if (!bXYOverride)
		{
			FinalVel.X += Velocity.X;
			FinalVel.Y += Velocity.Y;
		}
		if (!bZOverride)
		{
			FinalVel.Z += Velocity.Z;
		}

		CharacterMovement->Launch(FinalVel);

		OnLaunched(LaunchVelocity, bXYOverride, bZOverride);
	}
}


void APlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PrevCustomMode)
{
	if (!bPressedJump || !CharacterMovement->IsFalling())
	{
		ResetJumpState();
	}

	// Record jump force start time for proxies. Allows us to expire the jump even if not continually ticking down a timer.
	if (bProxyIsJumpForceApplied && CharacterMovement->IsFalling())
	{
		ProxyJumpForceStartedTime = GetWorld()->GetTimeSeconds();
	}

	K2_OnMovementModeChanged(PrevMovementMode, CharacterMovement->MovementMode, PrevCustomMode, CharacterMovement->CustomMovementMode);
	//MovementModeChangedDelegate.Broadcast((Acharacter)this, PrevMovementMode, PrevCustomMode);
}


/** Don't process landed notification if updating client position by replaying moves. 
 * Allow event to be called if Pawn was initially falling (before starting to replay moves), 
 * and this is going to cause him to land. . */
bool APlayerCharacter::ShouldNotifyLanded(const FHitResult& Hit)
{
	if (bClientUpdating && !bClientWasFalling)
	{
		return false;
	}

	// Just in case, only allow Landed() to be called once when replaying moves.
	bClientWasFalling = false;
	return true;
}

void APlayerCharacter::Jump()
{
	bPressedJump = true;
	JumpKeyHoldTime = 0.0f;
}

void APlayerCharacter::StopJumping()
{
	bPressedJump = false;
	ResetJumpState();
}

void APlayerCharacter::CheckJumpInput(float DeltaTime)
{
	JumpCurrentCountPreJump = JumpCurrentCount;

	if (CharacterMovement)
	{
		if (bPressedJump)
		{
			// If this is the first jump and we're already falling,
			// then increment the JumpCount to compensate.
			const bool bFirstJump = JumpCurrentCount == 0;
			if (bFirstJump && CharacterMovement->IsFalling())
			{
				JumpCurrentCount++;
			}

			const bool bDidJump = CanJump() && CharacterMovement->DoJump(bClientUpdating);
			if (bDidJump)
			{
				// Transition from not (actively) jumping to jumping.
				if (!bWasJumping)
				{
					JumpCurrentCount++;
					JumpForceTimeRemaining = GetJumpMaxHoldTime();
					OnJumped();
				}
			}

			bWasJumping = bDidJump;
		}
	}
}


void APlayerCharacter::ClearJumpInput(float DeltaTime)
{
	if (bPressedJump)
	{
		JumpKeyHoldTime += DeltaTime;

		// Don't disable bPressedJump right away if it's still held.
		// Don't modify JumpForceTimeRemaining because a frame of update may be remaining.
		if (JumpKeyHoldTime >= GetJumpMaxHoldTime())
		{
			bPressedJump = false;
		}
	}
	else
	{
		JumpForceTimeRemaining = 0.0f;
		bWasJumping = false;
	}
}

float APlayerCharacter::GetJumpMaxHoldTime() const
{
	return JumpMaxHoldTime;
}

//
// Static variables for networking.
//
static uint8 SavedMovementMode;

void APlayerCharacter::PreNetReceive()
{
	SavedMovementMode = ReplicatedMovementMode;
	Super::PreNetReceive();
}

void APlayerCharacter::PostNetReceive()
{
	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		CharacterMovement->bNetworkMovementModeChanged |= ((SavedMovementMode != ReplicatedMovementMode) || (CharacterMovement->PackNetworkMovementMode() != ReplicatedMovementMode));
		CharacterMovement->bNetworkUpdateReceived |= CharacterMovement->bNetworkMovementModeChanged || CharacterMovement->bJustTeleported;
	}

	Super::PostNetReceive();
}

void APlayerCharacter::OnRep_ReplicatedBasedMovement()
{	
	if (GetLocalRole() != ROLE_SimulatedProxy)
	{
		return;
	}

	// Skip base updates while playing root motion, it is handled inside of OnRep_RootMotion
	if (IsPlayingNetworkedRootMotionMontage())
	{
		return;
	}

	CharacterMovement->bNetworkUpdateReceived = true;
	TGuardValue<bool> bInBaseReplicationGuard(bInBaseReplication, true);

	const bool bBaseChanged = (BasedMovement.MovementBase != ReplicatedBasedMovement.MovementBase || BasedMovement.BoneName != ReplicatedBasedMovement.BoneName);
	if (bBaseChanged)
	{
		// Even though we will copy the replicated based movement info, we need to use SetBase() to set up tick dependencies and trigger notifications.
		SetBase(ReplicatedBasedMovement.MovementBase, ReplicatedBasedMovement.BoneName);
	}

	// Make sure to use the values of relative location/rotation etc from the server.
	BasedMovement = ReplicatedBasedMovement;

	if (ReplicatedBasedMovement.HasRelativeLocation())
	{
		// Update transform relative to movement base
		const FVector OldLocation = GetActorLocation();
		const FQuat OldRotation = GetActorQuat();
		MovementBaseUtility::GetMovementBaseTransform(ReplicatedBasedMovement.MovementBase, ReplicatedBasedMovement.BoneName, CharacterMovement->OldBaseLocation, CharacterMovement->OldBaseQuat);
		const FVector NewLocation = CharacterMovement->OldBaseLocation + ReplicatedBasedMovement.Location;
		FRotator NewRotation;

		if (ReplicatedBasedMovement.HasRelativeRotation())
		{
			// Relative location, relative rotation
			NewRotation = (FRotationMatrix(ReplicatedBasedMovement.Rotation) * FQuatRotationMatrix(CharacterMovement->OldBaseQuat)).Rotator();
			
			if (CharacterMovement->ShouldRemainVertical())
			{
				NewRotation.Pitch = 0.f;
				NewRotation.Roll = 0.f;
			}
		}
		else
		{
			// Relative location, absolute rotation
			NewRotation = ReplicatedBasedMovement.Rotation;
		}

		// When position or base changes, movement mode will need to be updated. This assumes rotation changes don't affect that.
		CharacterMovement->bJustTeleported |= (bBaseChanged || NewLocation != OldLocation);
		CharacterMovement->bNetworkSmoothingComplete = false;
		CharacterMovement->SmoothCorrection(OldLocation, OldRotation, NewLocation, NewRotation.Quaternion());
		OnUpdateSimulatedPosition(OldLocation, OldRotation);
	}
}

void APlayerCharacter::OnRep_ReplicatedMovement()
{
	if (CharacterMovement && (CharacterMovement->NetworkSmoothingMode == ENetworkSmoothingMode::Replay))
	{
		return;
	}

	// Skip standard position correction if we are playing root motion, OnRep_RootMotion will handle it.
	if (!IsPlayingNetworkedRootMotionMontage()) // animation root motion
	{
		if (!CharacterMovement || !CharacterMovement->CurrentRootMotion.HasActiveRootMotionSources()) // root motion sources
		{
			Super::OnRep_ReplicatedMovement();
		}
	}
}

void APlayerCharacter::OnRep_ReplayLastTransformUpdateTimeStamp()
{
	ReplicatedServerLastTransformUpdateTimeStamp = ReplayLastTransformUpdateTimeStamp;
}

/** Get FAnimMontageInstance playing RootMotion */
FAnimMontageInstance * APlayerCharacter::GetRootMotionAnimMontageInstance() const
{
	return (Mesh && Mesh->GetAnimInstance()) ? Mesh->GetAnimInstance()->GetRootMotionMontageInstance() : nullptr;
}

void APlayerCharacter::OnRep_RootMotion()
{
	if (CharacterMovement && (CharacterMovement->NetworkSmoothingMode == ENetworkSmoothingMode::Replay))
	{
		return;
	}

	if (GetLocalRole() == ROLE_SimulatedProxy)
	{

		UE_LOG(LogRootMotion, Log,  TEXT("APlayerCharacter::OnRep_RootMotion"));

		// Save received move in queue, we'll try to use it during Tick().
		if( RepRootMotion.bIsActive )
		{
			// Add new move
			RootMotionRepMoves.AddZeroed(1);
			FSimulatedRootMotionReplicatedMove& NewMove = RootMotionRepMoves.Last();
			NewMove.RootMotion = RepRootMotion;
			NewMove.Time = GetWorld()->GetTimeSeconds();
		}
		else
		{
			// Clear saved moves.
			RootMotionRepMoves.Empty();
		}

		if (CharacterMovement)
		{
			CharacterMovement->bNetworkUpdateReceived = true;
		}
	}
}

void APlayerCharacter::SimulatedRootMotionPositionFixup(float DeltaSeconds)
{
	const FAnimMontageInstance* ClientMontageInstance = GetRootMotionAnimMontageInstance();
	if( ClientMontageInstance && CharacterMovement && Mesh )
	{
		// Find most recent buffered move that we can use.
		const int32 MoveIndex = FindRootMotionRepMove(*ClientMontageInstance);
		if( MoveIndex != INDEX_NONE )
		{
			const FVector OldLocation = GetActorLocation();
			const FQuat OldRotation = GetActorQuat();
			// Move Actor back to position of that buffered move. (server replicated position).
			const FSimulatedRootMotionReplicatedMove& RootMotionRepMove = RootMotionRepMoves[MoveIndex];
			if( RestoreReplicatedMove(RootMotionRepMove) )
			{
				const float ServerPosition = RootMotionRepMove.RootMotion.Position;
				const float ClientPosition = ClientMontageInstance->GetPosition();
				const float DeltaPosition = (ClientPosition - ServerPosition);
				if( FMath::Abs(DeltaPosition) > KINDA_SMALL_NUMBER )
				{
					// Find Root Motion delta move to get back to where we were on the client.
					const FTransform LocalRootMotionTransform = ClientMontageInstance->Montage->ExtractRootMotionFromTrackRange(ServerPosition, ClientPosition);

					// Simulate Root Motion for delta move.
					if( CharacterMovement )
					{
						const float MontagePlayRate = ClientMontageInstance->GetPlayRate();
						// Guess time it takes for this delta track position, so we can get falling physics accurate.
						if (!FMath::IsNearlyZero(MontagePlayRate))
						{
							const float DeltaTime = DeltaPosition / MontagePlayRate;

							// Even with negative playrate deltatime should be positive.
							check(DeltaTime > 0.f);
							CharacterMovement->SimulateRootMotion(DeltaTime, LocalRootMotionTransform);

							// After movement correction, smooth out error in position if any.
							const FVector NewLocation = GetActorLocation();
							CharacterMovement->bNetworkSmoothingComplete = false;
							CharacterMovement->bJustTeleported |= (OldLocation != NewLocation);
							CharacterMovement->SmoothCorrection(OldLocation, OldRotation, NewLocation, GetActorQuat());
						}
					}
				}
			}

			// Delete this move and any prior one, we don't need them anymore.
			UE_LOG(LogRootMotion, Log,  TEXT("\tClearing old moves (%d)"), MoveIndex+1);
			RootMotionRepMoves.RemoveAt(0, MoveIndex+1);
		}
	}
}

int32 APlayerCharacter::FindRootMotionRepMove(const FAnimMontageInstance& ClientMontageInstance) const
{
	int32 FoundIndex = INDEX_NONE;

	// Start with most recent move and go back in time to find a usable move.
	for(int32 MoveIndex=RootMotionRepMoves.Num()-1; MoveIndex>=0; MoveIndex--)
	{
		if( CanUseRootMotionRepMove(RootMotionRepMoves[MoveIndex], ClientMontageInstance) )
		{
			FoundIndex = MoveIndex;
			break;
		}
	}

	UE_LOG(LogRootMotion, Log,  TEXT("\tAPlayerCharacter::FindRootMotionRepMove FoundIndex: %d, NumSavedMoves: %d"), FoundIndex, RootMotionRepMoves.Num());
	return FoundIndex;
}

bool APlayerCharacter::CanUseRootMotionRepMove(const FSimulatedRootMotionReplicatedMove& RootMotionRepMove, const FAnimMontageInstance& ClientMontageInstance) const
{
	// Ignore outdated moves.
	if( GetWorld()->TimeSince(RootMotionRepMove.Time) <= 0.5f )
	{
		// Make sure montage being played matched between client and server.
		if( RootMotionRepMove.RootMotion.AnimMontage && (RootMotionRepMove.RootMotion.AnimMontage == ClientMontageInstance.Montage) )
		{
			UAnimMontage * AnimMontage = ClientMontageInstance.Montage;
			const float ServerPosition = RootMotionRepMove.RootMotion.Position;
			const float ClientPosition = ClientMontageInstance.GetPosition();
			const float DeltaPosition = (ClientPosition - ServerPosition);
			const int32 CurrentSectionIndex = AnimMontage->GetSectionIndexFromPosition(ClientPosition);
			if( CurrentSectionIndex != INDEX_NONE )
			{
				const int32 NextSectionIndex = ClientMontageInstance.GetNextSectionID(CurrentSectionIndex);

				// We can only extract root motion if we are within the same section.
				// It's not trivial to jump through sections in a deterministic manner, but that is luckily not frequent. 
				const bool bSameSections = (AnimMontage->GetSectionIndexFromPosition(ServerPosition) == CurrentSectionIndex);
				// if we are looping and just wrapped over, skip. That's also not easy to handle and not frequent.
				const bool bHasLooped = (NextSectionIndex == CurrentSectionIndex) && (FMath::Abs(DeltaPosition) > (AnimMontage->GetSectionLength(CurrentSectionIndex) / 2.f));
				// Can only simulate forward in time, so we need to make sure server move is not ahead of the client.
				const bool bServerAheadOfClient = ((DeltaPosition * ClientMontageInstance.GetPlayRate()) < 0.f);

				UE_LOG(LogRootMotion, Log,  TEXT("\t\tAPlayerCharacter::CanUseRootMotionRepMove ServerPosition: %.3f, ClientPosition: %.3f, DeltaPosition: %.3f, bSameSections: %d, bHasLooped: %d, bServerAheadOfClient: %d"), 
					ServerPosition, ClientPosition, DeltaPosition, bSameSections, bHasLooped, bServerAheadOfClient);

				return bSameSections && !bHasLooped && !bServerAheadOfClient;
			}
		}
	}
	return false;
}

bool APlayerCharacter::RestoreReplicatedMove(const FSimulatedRootMotionReplicatedMove& RootMotionRepMove)
{
	UPrimitiveComponent* ServerBase = RootMotionRepMove.RootMotion.MovementBase;
	const FName ServerBaseBoneName = RootMotionRepMove.RootMotion.MovementBaseBoneName;

	// Relative Position
	if( RootMotionRepMove.RootMotion.bRelativePosition )
	{
		bool bSuccess = false;
		if( MovementBaseUtility::UseRelativeLocation(ServerBase) )
		{
			FVector BaseLocation;
			FQuat BaseRotation;
			MovementBaseUtility::GetMovementBaseTransform(ServerBase, ServerBaseBoneName, BaseLocation, BaseRotation);

			const FVector ServerLocation = BaseLocation + RootMotionRepMove.RootMotion.Location;
			FRotator ServerRotation;
			if (RootMotionRepMove.RootMotion.bRelativeRotation)
			{
				// Relative rotation
				ServerRotation = (FRotationMatrix(RootMotionRepMove.RootMotion.Rotation) * FQuatRotationTranslationMatrix(BaseRotation, FVector::ZeroVector)).Rotator();
			}
			else
			{
				// Absolute rotation
				ServerRotation = RootMotionRepMove.RootMotion.Rotation;
			}

			SetActorLocationAndRotation(ServerLocation, ServerRotation);
			bSuccess = true;
		}
		// If we received local space position, but can't resolve parent, then move can't be used. :(
		if( !bSuccess )
		{
			return false;
		}
	}
	// Absolute position
	else
	{
		FVector LocalLocation = FRepMovement::RebaseOntoLocalOrigin(RootMotionRepMove.RootMotion.Location, this);
		SetActorLocationAndRotation(LocalLocation, RootMotionRepMove.RootMotion.Rotation);
	}

	CharacterMovement->bJustTeleported = true;
	SetBase( ServerBase, ServerBaseBoneName );

	return true;
}

void APlayerCharacter::OnUpdateSimulatedPosition(const FVector& OldLocation, const FQuat& OldRotation)
{
	//SCOPE_CYCLE_COUNTER(STAT_CharacterOnNetUpdateSimulatedPosition);

	bSimGravityDisabled = false;
	const bool bLocationChanged = (OldLocation != GetActorLocation());
	if (bClientCheckEncroachmentOnNetUpdate)
	{	
		// Only need to check for encroachment when teleported without any velocity.
		// Normal movement pops the character out of geometry anyway, no use doing it before and after (with different rules).
		// Always consider Location as changed if we were spawned this tick as in that case our replicated Location was set as part of spawning, before PreNetReceive()
		if (CharacterMovement->Velocity.IsZero() && (bLocationChanged || CreationTime == GetWorld()->TimeSeconds))
		{
			if (GetWorld()->EncroachingBlockingGeometry(this, GetActorLocation(), GetActorRotation()))
			{
				bSimGravityDisabled = true;
			}
		}
	}
	CharacterMovement->bJustTeleported |= bLocationChanged;
	CharacterMovement->bNetworkUpdateReceived = true;
}

void APlayerCharacter::PostNetReceiveLocationAndRotation()
{
	if(GetLocalRole() == ROLE_SimulatedProxy)
	{
		// Don't change transform if using relative position (it should be nearly the same anyway, or base may be slightly out of sync)
		if (!ReplicatedBasedMovement.HasRelativeLocation())
		{
			const FRepMovement& ConstRepMovement = GetReplicatedMovement();
			const FVector OldLocation = GetActorLocation();
			const FVector NewLocation = FRepMovement::RebaseOntoLocalOrigin(ConstRepMovement.Location, this);
			const FQuat OldRotation = GetActorQuat();

			CharacterMovement->bNetworkSmoothingComplete = false;
			CharacterMovement->bJustTeleported |= (OldLocation != NewLocation);
			CharacterMovement->SmoothCorrection(OldLocation, OldRotation, NewLocation, ConstRepMovement.Rotation.Quaternion());
			OnUpdateSimulatedPosition(OldLocation, OldRotation);
		}
		CharacterMovement->bNetworkUpdateReceived = true;
	}
}

void APlayerCharacter::PreReplication( IRepChangedPropertyTracker & ChangedPropertyTracker )
{
	Super::PreReplication( ChangedPropertyTracker );

	if (CharacterMovement->CurrentRootMotion.HasActiveRootMotionSources() || IsPlayingNetworkedRootMotionMontage())
	{
		const FAnimMontageInstance* RootMotionMontageInstance = GetRootMotionAnimMontageInstance();

		RepRootMotion.bIsActive = true;
		// Is position stored in local space?
		RepRootMotion.bRelativePosition = BasedMovement.HasRelativeLocation();
		RepRootMotion.bRelativeRotation = BasedMovement.HasRelativeRotation();
		RepRootMotion.Location			= RepRootMotion.bRelativePosition ? BasedMovement.Location : FRepMovement::RebaseOntoZeroOrigin(GetActorLocation(), GetWorld()->OriginLocation);
		RepRootMotion.Rotation			= RepRootMotion.bRelativeRotation ? BasedMovement.Rotation : GetActorRotation();
		RepRootMotion.MovementBase		= BasedMovement.MovementBase;
		RepRootMotion.MovementBaseBoneName = BasedMovement.BoneName;
		if (RootMotionMontageInstance)
		{
			RepRootMotion.AnimMontage		= RootMotionMontageInstance->Montage;
			RepRootMotion.Position			= RootMotionMontageInstance->GetPosition();
		}
		else
		{
			RepRootMotion.AnimMontage = nullptr;
		}

		RepRootMotion.AuthoritativeRootMotion = CharacterMovement->CurrentRootMotion;
		RepRootMotion.Acceleration = CharacterMovement->GetCurrentAcceleration();
		RepRootMotion.LinearVelocity = CharacterMovement->Velocity;

		DOREPLIFETIME_ACTIVE_OVERRIDE( APlayerCharacter, RepRootMotion, true );
	}
	else
	{
		RepRootMotion.Clear();

		DOREPLIFETIME_ACTIVE_OVERRIDE( APlayerCharacter, RepRootMotion, false );
	}

	bProxyIsJumpForceApplied = (JumpForceTimeRemaining > 0.0f);
	ReplicatedMovementMode = CharacterMovement->PackNetworkMovementMode();	
	ReplicatedBasedMovement = BasedMovement;

	// Optimization: only update and replicate these values if they are actually going to be used.
	if (BasedMovement.HasRelativeLocation())
	{
		// When velocity becomes zero, force replication so the position is updated to match the server (it may have moved due to simulation on the client).
		ReplicatedBasedMovement.bServerHasVelocity = !CharacterMovement->Velocity.IsZero();

		// Make sure absolute rotations are updated in case rotation occurred after the base info was saved.
		if (!BasedMovement.HasRelativeRotation())
		{
			ReplicatedBasedMovement.Rotation = GetActorRotation();
		}
	}

	// Save bandwidth by not replicating this value unless it is necessary, since it changes every update.
	if ((CharacterMovement->NetworkSmoothingMode == ENetworkSmoothingMode::Linear) || CharacterMovement->bNetworkAlwaysReplicateTransformUpdateTimestamp)
	{
		ReplicatedServerLastTransformUpdateTimeStamp = CharacterMovement->GetServerLastTransformUpdateTimeStamp();
	}
	else
	{
		ReplicatedServerLastTransformUpdateTimeStamp = 0.f;
	}
}

void APlayerCharacter::PreReplicationForReplay(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplicationForReplay(ChangedPropertyTracker);

	const UWorld* World = GetWorld();
	if (World)
	{
		// On client replays, our view pitch will be set to 0 as by default we do not replicate
		// pitch for owners, just for simulated. So instead push our rotation into the sampler
		if (World->IsRecordingClientReplay() && Controller != nullptr && GetLocalRole() == ROLE_AutonomousProxy && GetNetMode() == NM_Client)
		{
			SetRemoteViewPitch(Controller->GetControlRotation().Pitch);
		}

		ReplayLastTransformUpdateTimeStamp = World->GetTimeSeconds();
	}
}

void APlayerCharacter::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );

	DISABLE_REPLICATED_PROPERTY(APlayerCharacter, JumpMaxHoldTime);
	DISABLE_REPLICATED_PROPERTY(APlayerCharacter, JumpMaxCount);

	DOREPLIFETIME_CONDITION( APlayerCharacter, RepRootMotion,						COND_SimulatedOnly );
	DOREPLIFETIME_CONDITION( APlayerCharacter, ReplicatedBasedMovement,			COND_SimulatedOnly );
	DOREPLIFETIME_CONDITION( APlayerCharacter, ReplicatedServerLastTransformUpdateTimeStamp, COND_SimulatedOnlyNoReplay );
	DOREPLIFETIME_CONDITION( APlayerCharacter, ReplicatedMovementMode,			COND_SimulatedOnly );
	DOREPLIFETIME_CONDITION( APlayerCharacter, bIsCrouched,						COND_SimulatedOnly );
	DOREPLIFETIME_CONDITION( APlayerCharacter, bProxyIsJumpForceApplied,			COND_SimulatedOnly );
	DOREPLIFETIME_CONDITION( APlayerCharacter, AnimRootMotionTranslationScale,	COND_SimulatedOnly );
	DOREPLIFETIME_CONDITION( APlayerCharacter, ReplayLastTransformUpdateTimeStamp, COND_ReplayOnly );
}

bool APlayerCharacter::IsPlayingRootMotion() const
{
	if (Mesh)
	{
		return Mesh->IsPlayingRootMotion();
	}
	return false;
}

bool APlayerCharacter::HasAnyRootMotion() const
{
	return CharacterMovement ? CharacterMovement->HasRootMotionSources() : false;
}

bool APlayerCharacter::IsPlayingNetworkedRootMotionMontage() const
{
	if (Mesh)
	{
		return Mesh->IsPlayingNetworkedRootMotionMontage();
	}
	return false;
}

void APlayerCharacter::SetAnimRootMotionTranslationScale(float InAnimRootMotionTranslationScale)
{
	AnimRootMotionTranslationScale = InAnimRootMotionTranslationScale;
}

float APlayerCharacter::GetAnimRootMotionTranslationScale() const
{
	return AnimRootMotionTranslationScale;
}

float APlayerCharacter::PlayAnimMontage(class UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	UAnimInstance * AnimInstance = (Mesh)? Mesh->GetAnimInstance() : nullptr; 
	if( AnimMontage && AnimInstance )
	{
		float const Duration = AnimInstance->Montage_Play(AnimMontage, InPlayRate);

		if (Duration > 0.f)
		{
			// Start at a given Section.
			if( StartSectionName != NAME_None )
			{
				AnimInstance->Montage_JumpToSection(StartSectionName, AnimMontage);
			}

			return Duration;
		}
	}	

	return 0.f;
}

void APlayerCharacter::StopAnimMontage(class UAnimMontage* AnimMontage)
{
	UAnimInstance * AnimInstance = (Mesh)? Mesh->GetAnimInstance() : nullptr; 
	UAnimMontage * MontageToStop = (AnimMontage)? AnimMontage : GetCurrentMontage();
	bool bShouldStopMontage =  AnimInstance && MontageToStop && !AnimInstance->Montage_GetIsStopped(MontageToStop);

	if ( bShouldStopMontage )
	{
		AnimInstance->Montage_Stop(MontageToStop->BlendOut.GetBlendTime(), MontageToStop);
	}
}

class UAnimMontage * APlayerCharacter::GetCurrentMontage() const
{
	UAnimInstance * AnimInstance = (Mesh)? Mesh->GetAnimInstance() : nullptr; 
	if ( AnimInstance )
	{
		return AnimInstance->GetCurrentActiveMontage();
	}

	return nullptr;
}

void APlayerCharacter::ClientCheatWalk_Implementation()
{
#if !UE_BUILD_SHIPPING
	SetActorEnableCollision(true);
	if (CharacterMovement)
	{
		CharacterMovement->bCheatFlying = false;
		CharacterMovement->SetMovementMode(MOVE_Falling);
	}
#endif
}

void APlayerCharacter::ClientCheatFly_Implementation()
{
#if !UE_BUILD_SHIPPING
	SetActorEnableCollision(true);
	if (CharacterMovement)
	{
		CharacterMovement->bCheatFlying = true;
		CharacterMovement->SetMovementMode(MOVE_Flying);
	}
#endif
}

void APlayerCharacter::ClientCheatGhost_Implementation()
{
#if !UE_BUILD_SHIPPING
	SetActorEnableCollision(false);
	if (CharacterMovement)
	{
		CharacterMovement->bCheatFlying = true;
		CharacterMovement->SetMovementMode(MOVE_Flying);
	}
#endif
}

void APlayerCharacter::RootMotionDebugClientPrintOnScreen_Implementation(const FString& InString)
{
#if ROOT_MOTION_DEBUG
	RootMotionSourceDebug::PrintOnScreenServerMsg(InString);
#endif
}


// ServerMovePacked
void APlayerCharacter::ServerMovePacked_Implementation(const FCharacterServerMovePackedBits& PackedBits)
{
	GetCharacterMovement()->ServerMovePacked_ServerReceive(PackedBits);
}

bool APlayerCharacter::ServerMovePacked_Validate(const FCharacterServerMovePackedBits& PackedBits)
{
	// Can't really validate the bit stream without unpacking, and that is done in ServerMovePacked_ServerReceive() and can be rejected after unpacking.
	return true;
}

// ServerMove
void APlayerCharacter::ServerMove_Implementation(float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 CompressedMoveFlags, uint8 ClientRoll, uint32 View, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode)
{
	GetCharacterMovement()->ServerMove_Implementation(TimeStamp, InAccel, ClientLoc, CompressedMoveFlags, ClientRoll, View, ClientMovementBase, ClientBaseBoneName, ClientMovementMode);
}

bool APlayerCharacter::ServerMove_Validate(float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 CompressedMoveFlags, uint8 ClientRoll, uint32 View, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode)
{
	return GetCharacterMovement()->ServerMove_Validate(TimeStamp, InAccel, ClientLoc, CompressedMoveFlags, ClientRoll, View, ClientMovementBase, ClientBaseBoneName, ClientMovementMode);
}

// ServerMoveNoBase
void APlayerCharacter::ServerMoveNoBase_Implementation(float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 CompressedMoveFlags, uint8 ClientRoll, uint32 View, uint8 ClientMovementMode)
{
	GetCharacterMovement()->ServerMove_Implementation(TimeStamp, InAccel, ClientLoc, CompressedMoveFlags, ClientRoll, View, /*ClientMovementBase=*/ nullptr, /*ClientBaseBoneName=*/ NAME_None, ClientMovementMode);
}

bool APlayerCharacter::ServerMoveNoBase_Validate(float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 CompressedMoveFlags, uint8 ClientRoll, uint32 View, uint8 ClientMovementMode)
{
	return GetCharacterMovement()->ServerMove_Validate(TimeStamp, InAccel, ClientLoc, CompressedMoveFlags, ClientRoll, View, /*ClientMovementBase=*/ nullptr, /*ClientBaseBoneName=*/ NAME_None, ClientMovementMode);
}

// ServerMoveDual
void APlayerCharacter::ServerMoveDual_Implementation(float TimeStamp0, FVector_NetQuantize10 InAccel0, uint8 PendingFlags, uint32 View0, float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 NewFlags, uint8 ClientRoll, uint32 View, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode)
{
	GetCharacterMovement()->ServerMoveDual_Implementation(TimeStamp0, InAccel0, PendingFlags, View0, TimeStamp, InAccel, ClientLoc, NewFlags, ClientRoll, View, ClientMovementBase, ClientBaseBoneName, ClientMovementMode);
}

bool APlayerCharacter::ServerMoveDual_Validate(float TimeStamp0, FVector_NetQuantize10 InAccel0, uint8 PendingFlags, uint32 View0, float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 NewFlags, uint8 ClientRoll, uint32 View, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode)
{
	return GetCharacterMovement()->ServerMoveDual_Validate(TimeStamp0, InAccel0, PendingFlags, View0, TimeStamp, InAccel, ClientLoc, NewFlags, ClientRoll, View, ClientMovementBase, ClientBaseBoneName, ClientMovementMode);
}

// ServerMoveDualNoBase
void APlayerCharacter::ServerMoveDualNoBase_Implementation(float TimeStamp0, FVector_NetQuantize10 InAccel0, uint8 PendingFlags, uint32 View0, float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 NewFlags, uint8 ClientRoll, uint32 View, uint8 ClientMovementMode)
{
	GetCharacterMovement()->ServerMoveDual_Implementation(TimeStamp0, InAccel0, PendingFlags, View0, TimeStamp, InAccel, ClientLoc, NewFlags, ClientRoll, View, /*ClientMovementBase=*/ nullptr, /*ClientBaseBoneName=*/ NAME_None, ClientMovementMode);
}

bool APlayerCharacter::ServerMoveDualNoBase_Validate(float TimeStamp0, FVector_NetQuantize10 InAccel0, uint8 PendingFlags, uint32 View0, float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 NewFlags, uint8 ClientRoll, uint32 View, uint8 ClientMovementMode)
{
	return GetCharacterMovement()->ServerMoveDual_Validate(TimeStamp0, InAccel0, PendingFlags, View0, TimeStamp, InAccel, ClientLoc, NewFlags, ClientRoll, View, /*ClientMovementBase=*/ nullptr, /*ClientBaseBoneName=*/ NAME_None, ClientMovementMode);
}

// ServerMoveDualHybridRootMotion
void APlayerCharacter::ServerMoveDualHybridRootMotion_Implementation(float TimeStamp0, FVector_NetQuantize10 InAccel0, uint8 PendingFlags, uint32 View0, float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 NewFlags, uint8 ClientRoll, uint32 View, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode)
{
	GetCharacterMovement()->ServerMoveDualHybridRootMotion_Implementation(TimeStamp0, InAccel0, PendingFlags, View0, TimeStamp, InAccel, ClientLoc, NewFlags, ClientRoll, View, ClientMovementBase, ClientBaseBoneName, ClientMovementMode);
}

bool APlayerCharacter::ServerMoveDualHybridRootMotion_Validate(float TimeStamp0, FVector_NetQuantize10 InAccel0, uint8 PendingFlags, uint32 View0, float TimeStamp, FVector_NetQuantize10 InAccel, FVector_NetQuantize100 ClientLoc, uint8 NewFlags, uint8 ClientRoll, uint32 View, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode)
{
	return GetCharacterMovement()->ServerMoveDualHybridRootMotion_Validate(TimeStamp0, InAccel0, PendingFlags, View0, TimeStamp, InAccel, ClientLoc, NewFlags, ClientRoll, View, ClientMovementBase, ClientBaseBoneName, ClientMovementMode);
}

// ServerMoveOld
void APlayerCharacter::ServerMoveOld_Implementation(float OldTimeStamp, FVector_NetQuantize10 OldAccel, uint8 OldMoveFlags)
{
	GetCharacterMovement()->ServerMoveOld_Implementation(OldTimeStamp, OldAccel, OldMoveFlags);
}

bool APlayerCharacter::ServerMoveOld_Validate(float OldTimeStamp, FVector_NetQuantize10 OldAccel, uint8 OldMoveFlags)
{
	return GetCharacterMovement()->ServerMoveOld_Validate(OldTimeStamp, OldAccel, OldMoveFlags);
}

// ClientMoveResponsePacked
void APlayerCharacter::ClientMoveResponsePacked_Implementation(const FCharacterMoveResponsePackedBits& PackedBits)
{
	GetCharacterMovement()->MoveResponsePacked_ClientReceive(PackedBits);
}

bool APlayerCharacter::ClientMoveResponsePacked_Validate(const FCharacterMoveResponsePackedBits& PackedBits)
{
	// Can't really validate the bit stream without unpacking, and that is done in MoveResponsePacked_ClientReceive() and can be rejected after unpacking.
	return true;
}

// ClientAckGoodMove
void APlayerCharacter::ClientAckGoodMove_Implementation(float TimeStamp)
{
	GetCharacterMovement()->ClientAckGoodMove_Implementation(TimeStamp);
}

// ClientAdjustPosition
void APlayerCharacter::ClientAdjustPosition_Implementation(float TimeStamp, FVector NewLoc, FVector NewVel, UPrimitiveComponent* NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode)
{
	GetCharacterMovement()->ClientAdjustPosition_Implementation(TimeStamp, NewLoc, NewVel, NewBase, NewBaseBoneName, bHasBase, bBaseRelativePosition, ServerMovementMode);
}

// ClientVeryShortAdjustPosition
void APlayerCharacter::ClientVeryShortAdjustPosition_Implementation(float TimeStamp, FVector NewLoc, UPrimitiveComponent* NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode)
{
	GetCharacterMovement()->ClientVeryShortAdjustPosition_Implementation(TimeStamp, NewLoc, NewBase, NewBaseBoneName, bHasBase, bBaseRelativePosition, ServerMovementMode);
}

// ClientAdjustRootMotionPosition
void APlayerCharacter::ClientAdjustRootMotionPosition_Implementation(float TimeStamp, float ServerMontageTrackPosition, FVector ServerLoc, FVector_NetQuantizeNormal ServerRotation, float ServerVelZ, UPrimitiveComponent* ServerBase, FName ServerBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode)
{
	GetCharacterMovement()->ClientAdjustRootMotionPosition_Implementation(TimeStamp, ServerMontageTrackPosition, ServerLoc, ServerRotation, ServerVelZ, ServerBase, ServerBoneName, bHasBase, bBaseRelativePosition, ServerMovementMode);
}

// ClientAdjustRootMotionSourcePosition
void APlayerCharacter::ClientAdjustRootMotionSourcePosition_Implementation(float TimeStamp, FRootMotionSourceGroup ServerRootMotion, bool bHasAnimRootMotion, float ServerMontageTrackPosition, FVector ServerLoc, FVector_NetQuantizeNormal ServerRotation, float ServerVelZ, UPrimitiveComponent* ServerBase, FName ServerBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode)
{
	GetCharacterMovement()->ClientAdjustRootMotionSourcePosition_Implementation(TimeStamp, ServerRootMotion, bHasAnimRootMotion, ServerMontageTrackPosition, ServerLoc, ServerRotation, ServerVelZ, ServerBase, ServerBoneName, bHasBase, bBaseRelativePosition, ServerMovementMode);
}

void 
APlayerCharacter::Tick(float DeltaTime)
{
	//oxigen-=DeltaTime;
	//if(health > Cast<Udamagable>(GetComponentByClass(Udamagable::StaticClass()))->getVidaPercentage()){
	//	health -= DeltaTime*.125f;
	//}
	//
	//if(oxigen<0){
	//	die();
	//}
	//
	//if(shooting && actualWeapon){
	//	actualWeapon->delta = DeltaTime;
	//	actualWeapon->shotting();
	//	
	//}
	//
	//if(isDead){
	//	deadTimer += DeltaTime;
	//	if(deadTimer>1){
	//		UGameplayStatics::OpenLevel(GetWorld(),"workbench");
	//	}
	//}
	//
	////GetMovementComponent()->Velocity =  FVector::ZeroVector;
	//FHitResult result;
	//auto feetPos = feets+GetActorLocation();
  //GetWorld()->LineTraceSingleByChannel(result,feetPos,feetPos-FVector(0,0,collicionDistance),ECollisionChannel::ECC_Visibility);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%f %f %f"), result.ImpactPoint.X,result.ImpactPoint.Y,result.ImpactPoint.Z));
	//CharacterMovement;
	
	//auto castComp = Cast<UMyCharacterMovementComponent>(comp);
	//
	//if(castComp)
	//castComp->isFlying = result.ImpactPoint==FVector::ZeroVector; 
	
	
}

//#include "PlayerCharacter.h"
//#include "voidBastardsProjectile.h"
//#include "Animation/AnimInstance.h"
//#include "Camera/CameraComponent.h"
//#include "Components/CapsuleComponent.h"
//#include "Components/InputComponent.h"
//#include "GameFramework/InputSettings.h"
//#include "HeadMountedDisplayFunctionLibrary.h"
//#include "Kismet/GameplayStatics.h"
//#include "MotionControllerComponent.h"
//#include "weapons/Regulator.h"
//#include "weapons/Device.h"
//#include "weapons/BushWhacker.h"
//#include "damagable.h"
//#include "Part.h"
//#include "ExitDoor.h"
//#include "MyCharacterMovementComponent.h"
//#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
//#include "GameFramework/CharacterMovementComponent.h"
//#include "Movement\Movement_C.h"
//
//// Sets default values
//APlayerCharacter::APlayerCharacter()
//{
// 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
//	PrimaryActorTick.bCanEverTick = true;
//
//	MovementComp = CreateDefaultSubobject<UMovement_C>("MovementCmp");
//
//}
//
//// Called when the game starts or when spawned
//void APlayerCharacter::BeginPlay()
//{
//	Super::BeginPlay();
//	MovementComp->InitComponent(GetActorRightVector(), feets);
//	
//}
//
//// Called every frame
//void APlayerCharacter::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//	if (MovementComp)
//  {
//    MovementComp->Move(SumForces);
//    SumForces = FVector(0.0f, 0.0f, 0.0f);
//    
//    
//  }
//}
//
//// Called to bind functionality to input
//void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
//	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveHorizontal);
//}
//
//
////APlayerCharacter::APlayerCharacter()
////{
////	// Set size for collision capsule
////	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
////
////	//GetMovementComponent
////
////	// set our turn rates for input
////	BaseTurnRate = 45.f;
////	BaseLookUpRate = 45.f;
////
////	// Create a CameraComponent	
////	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
////	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
////	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
////	FirstPersonCameraComponent->bUsePawnControlRotation = true;
////
////	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
////	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
////	Mesh1P->SetOnlyOwnerSee(true);
////	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
////	Mesh1P->bCastDynamicShadow = false;
////	Mesh1P->CastShadow = false;
////	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
////	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
////
////	// Create a gun mesh component
////	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
////	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
////	FP_Gun->bCastDynamicShadow = false;
////	FP_Gun->CastShadow = false;
////	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
////	FP_Gun->SetupAttachment(RootComponent);
////
////	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
////	FP_MuzzleLocation->SetupAttachment(FP_Gun);
////	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
////
////	// Default offset from the character location for projectiles to spawn
////	GunOffset = FVector(100.0f, 0.0f, 10.0f);
////
////	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
////	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.
////
////	// Create VR Controllers.
////	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
////	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
////	R_MotionController->SetupAttachment(RootComponent);
////	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
////	L_MotionController->SetupAttachment(RootComponent);
////
////	// Create a gun and attach it to the right-hand VR controller.
////	// Create a gun mesh component
////	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
////	VR_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
////	VR_Gun->bCastDynamicShadow = false;
////	VR_Gun->CastShadow = false;
////	VR_Gun->SetupAttachment(R_MotionController);
////	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
////
////	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
////	VR_MuzzleLocation->SetupAttachment(VR_Gun);
////	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
////	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.
////
////	firearm = CreateDefaultSubobject<URegulator>("fire arm");
////	device = CreateDefaultSubobject<UDevice>("device");
////	indirect = CreateDefaultSubobject<UBushWhacker>("indirect");
////	actualWeapon = firearm;
////	// Uncomment the following line to turn motion controllers on by default:
////	//bUsingMotionControllers = true;
////
////	damagable = CreateDefaultSubobject<Udamagable>(TEXT("damagable"));
////}
//
////void APlayerCharacter::BeginPlay()
////{
////	// Call the base class  
////	Super::BeginPlay();
////
////	//size_t id = (size_t)GetMovementComponent();
////	//
////	//size_t* scaner = (size_t*)this;
////	//
////	//while(true){
////  //  if(id==*scaner){
////  //    break;
////  //  }
////  //  ++scaner;
////  //}
////	//auto comp = new UMyCharacterMovementComponent;
////  //*scaner = (size_t)();
////
////	//firearm->character = this;
////	//
////	//device->character = this;
////	//
////	//indirect->character = this;
////
////	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
////	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
////
////	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
////	if (bUsingMotionControllers)
////	{
////		VR_Gun->SetHiddenInGame(false, true);
////		Mesh1P->SetHiddenInGame(true, true);
////	}
////	else
////	{
////		VR_Gun->SetHiddenInGame(true, true);
////		Mesh1P->SetHiddenInGame(false, true);
////	}
////}
//
////void 
////APlayerCharacter::Tick(float DeltaTime)
////{
////	oxigen-=DeltaTime;
////	if(health > damagable->getVidaPercentage()){
////		health -= DeltaTime*.125f;
////	}
////
////	if(oxigen<0){
////		die();
////	}
////
////	if(shooting && actualWeapon){
////		actualWeapon->delta = DeltaTime;
////		actualWeapon->shotting();
////		
////	}
////
////	if(isDead){
////		deadTimer += DeltaTime;
////		if(deadTimer>1){
////			UGameplayStatics::OpenLevel(GetWorld(),"workbench");
////		}
////	}
////	
////	//GetMovementComponent()->Velocity =  FVector::ZeroVector;
////	FHitResult result;
////	auto feetPos = feets+GetActorLocation();
////  GetWorld()->LineTraceSingleByChannel(result,feetPos,feetPos-FVector(0,0,collicionDistance),ECollisionChannel::ECC_Visibility);
////	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%f %f %f"), result.ImpactPoint.X,result.ImpactPoint.Y,result.ImpactPoint.Z));
////	//CharacterMovement;
////	
////	//auto castComp = Cast<UMyCharacterMovementComponent>(comp);
////	//
////	//if(castComp)
////	//castComp->isFlying = result.ImpactPoint==FVector::ZeroVector; 
////	
////	
////}
//
//
//float APlayerCharacter::getOxigenPercentage()
//{
//	return oxigen/maxOxigen;
//}
//
////////////////////////////////////////////////////////////////////////////
//// Input
//
////void 
////APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
////{
////	// set up gameplay key bindings
////	//check(PlayerInputComponent);
////	//
////	//// Bind jump events
////	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::Jump);
////	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::StopJumping);
////	//
////	//// Bind fire event
////	//PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacter::OnFire);
////	//PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayerCharacter::OnEndFire);
////	//
////	//PlayerInputComponent->BindAction("Use", IE_Pressed, this, &APlayerCharacter::use);
////	//// Enable touchscreen input
////	//EnableTouchscreenMovement(PlayerInputComponent);
////	//
////	//PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &APlayerCharacter::OnResetVR);
////	//
////	//// Bind movement events
////	//PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
////	//PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
////	//
////	//// We have 2 versions of the rotation bindings to handle different kinds of devices differently
////	//// "turn" handles devices that provide an absolute delta, such as a mouse.
////	//// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
////	//PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
////	//PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRate);
////	//PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
////	//PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpAtRate);
////}
//
//void APlayerCharacter::OnFire()
//{
//	if(actualWeapon)
//	actualWeapon->use();
//
//	shooting = true;
//
//	
//
//	// try and fire a projectile
//	/*
//	if (ProjectileClass != nullptrUMyUserWidget
//	{
//		UWorld* const World = GetWorld();
//		if (World != nullptr)
//		{
//			if (bUsingMotionControllers)
//			{
//				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
//				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
//				World->SpawnActor<AvoidBastardsProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
//			}
//			else
//			{
//				const FRotator SpawnRotation = GetControlRotation();
//				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
//				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);
//
//				//Set Spawn Collision Handling Override
//				FActorSpawnParameters ActorSpawnParams;
//				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
//
//				// spawn the projectile at the muzzle
//				World->SpawnActor<AvoidBastardsProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
//			}
//		}
//	}
//
//	// try and play the sound if specified
//	if (FireSound != nullptr)
//	{
//		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
//	}
//
//	// try and play a firing animation if specified
//	if (FireAnimation != nullptr)
//	{
//		// Get the animation object for the arms mesh
//		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
//		if (AnimInstance != nullptr)
//		{
//			AnimInstance->Montage_Play(FireAnimation, 1.f);
//		}
//	}
//	*/
//}
//
//void APlayerCharacter::OnEndFire()
//{
//	if(actualWeapon)
//	actualWeapon->stopShotting();
//
//	shooting = false;
//}
//
//void APlayerCharacter::OnResetVR()
//{
//	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
//}
//
//void APlayerCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if (TouchItem.bIsPressed == true)
//	{
//		return;
//	}
//	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
//	{
//		OnFire();
//	}
//	TouchItem.bIsPressed = true;
//	TouchItem.FingerIndex = FingerIndex;
//	TouchItem.Location = Location;
//	TouchItem.bMoved = false;
//}
//
//void APlayerCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if (TouchItem.bIsPressed == false)
//	{
//		return;
//	}
//	TouchItem.bIsPressed = false;
//}
//
//void APlayerCharacter::use()
//{
//	auto dir = UGameplayStatics::GetPlayerCameraManager(GetWorld(),0)->GetActorForwardVector();
//	auto start = GetActorLocation();
//  auto end = start + dir*1000.f;
//  FHitResult result;
//  GetWorld()->LineTraceSingleByChannel(result,start,end,ECollisionChannel::ECC_Visibility);
//	auto actor = Cast<AExitDoor>(result.GetActor());
//	if(actor){
//		exitLevel();
//		UGameplayStatics::OpenLevel(GetWorld(),"workbench");
//	}
//}
//
////Commenting this section out to be consistent with FPS BP template.
////This allows the user to turn without using the right virtual joystick
//
////void APlayerCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
////{
////	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
////	{
////		if (TouchItem.bIsPressed)
////		{
////			if (GetWorld() != nullptr)
////			{
////				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
////				if (ViewportClient != nullptr)
////				{
////					FVector MoveDelta = Location - TouchItem.Location;
////					FVector2D ScreenSize;
////					ViewportClient->GetViewportSize(ScreenSize);
////					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
////					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
////					{
////						TouchItem.bMoved = true;
////						float Value = ScaledDelta.X * BaseTurnRate;
////						AddControllerYawInput(Value);
////					}
////					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
////					{
////						TouchItem.bMoved = true;
////						float Value = ScaledDelta.Y * BaseTurnRate;
////						AddControllerPitchInput(Value);
////					}
////					TouchItem.Location = Location;
////				}
////				TouchItem.Location = Location;
////			}
////		}
////	}
////}
//
//void APlayerCharacter::MoveForward(float Value)
//{
//	if (Value != 0.0f)
//	{
//		// add movement in that direction
//		AddMovementInput(GetActorForwardVector(), Value);
//	}
//}
//
////void APlayerCharacter::MoveRight(float Value)
////{
////	if (Value != 0.0f)
////	{
////		// add movement in that direction
////		AddMovementInput(GetActorRightVector(), Value);
////	}
////}
//
//void APlayerCharacter::TurnAtRate(float Rate)
//{
//	// calculate delta for this frame from the rate information
//	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
//}
//
//void APlayerCharacter::LookUpAtRate(float Rate)
//{
//	// calculate delta for this frame from the rate information
//	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
//}
//
//bool APlayerCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
//{
//	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
//	{
//		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &APlayerCharacter::BeginTouch);
//		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &APlayerCharacter::EndTouch);
//
//		//Commenting this out to be more consistent with FPS BP template.
//		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &APlayerCharacter::TouchUpdate);
//		return true;
//	}
//	
//	return false;
//}
//
//void APlayerCharacter::attachWeapon(UWeapon* weapon)
//{
//	//if(!weapon){
//	//	return;
//	//}
//	//
//	//weapon->character = this;
//}
//
//void APlayerCharacter::MoveForward(float value)
//{
//  if (value != 0 )
//  {
//    FVector OrientationVector = SpringArm->GetComponentRotation().Vector();
//    FVector front = FVector(OrientationVector.X, OrientationVector.Y, 0.0f).GetSafeNormal();
//    SumForces += front * value;
//  }
//}
//
//void APlayerCharacter::MoveHorizontal(float value)
//{
//  if (value != 0 )
//  {
//    FVector OrientationVector = SpringArm->GetComponentRotation().Vector();
//    FVector right = FVector(-OrientationVector.Y, OrientationVector.X, 0.0f).GetSafeNormal();
//    SumForces += right * value;
//  }
//}