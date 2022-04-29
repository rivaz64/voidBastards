// Fill out your copyright notice in the Description page of Project Settings.


#include "explotion.h"
#include "Kismet/GameplayStatics.h"
#include "damagable.h"

// Sets default values
Aexplotion::Aexplotion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void Aexplotion::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),AActor::StaticClass(),found);
	for(AActor* actor : found){
		auto component = actor->GetComponentByClass(Udamagable::StaticClass());
		if(component&&FVector::Dist(GetActorLocation(),actor->GetActorLocation())<radius){
			Cast<Udamagable>(component)->hitted(damage);
		}
	}
}

// Called every frame
void Aexplotion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

