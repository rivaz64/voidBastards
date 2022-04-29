// Fill out your copyright notice in the Description page of Project Settings.


#include "Throwable.h"
#include "Kismet/GameplayStatics.h"
#include "../Enemy.h"
#include "../damagable.h"

// Sets default values
AThrowable::AThrowable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
  mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
	RootComponent = mesh;

}

// Called when the game starts or when spawned
void AThrowable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AThrowable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AThrowable::explote()
{
	TArray<AActor*> found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),AActor::StaticClass(),found);
	for(AActor* actor : found){
		if(actor == ThrowedBy) continue;
		auto component = actor->GetComponentByClass(Udamagable::StaticClass());
		if(component&&FVector::Dist(GetActorLocation(),actor->GetActorLocation())<radius){
			Cast<Udamagable>(component)->hitted(damage);
		}
	}
	this->Destroy();
}

