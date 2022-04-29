// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"
#include "Kismet/GameplayStatics.h"
#include "voidBastardsCharacter.h"
#include "weapons/Throwable.h"
#include "damagable.h"
// Sets default values
ATurret::ATurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Root"));
	rotable = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rotable"));
	rotable->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto damag = Cast<Udamagable>(GetComponentByClass(Udamagable::StaticClass()));
	if(damag->vida<=0)return;
	auto player = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
		auto dist = player->GetActorLocation()-GetActorLocation();
		float angle = FMath::RadiansToDegrees(atan2(dist.Y,dist.X));
		rotable->SetWorldRotation(FRotator(0,angle-90,0));

	if(activated){

		time+=DeltaTime;
		if(time > timer){
			time = 0;
			FTransform trans;
			auto dir = dist.GetSafeNormal();
			dir.Z = 0;
			trans.SetLocation(dir*100+GetActorLocation()+FVector(0,0,height));
			auto bulletShooted = GetWorld()->SpawnActor<AThrowable>(throwable,trans);
			bulletShooted->mesh->AddImpulse(dir*impulse);
		}
	}
	
}

