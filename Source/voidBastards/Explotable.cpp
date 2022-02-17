// Fill out your copyright notice in the Description page of Project Settings.


#include "Explotable.h"
#include "Enemy.h"
#include "voidBastardsCharacter.h"

// Sets default values for this component's properties
UExplotable::UExplotable()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UExplotable::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UExplotable::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void
UExplotable::enterRange(AActor* actor){
  inRange.Add(actor);
}

void 
UExplotable::exitRange(AActor* actor)
{
  inRange.Remove(actor);
}

void 
UExplotable::explode(AActor* actor)
{
	/*Cast()
  player = Cast<AvoidBastardsCharacter>(actor);
  if(player) return;
  for(AActor* actor:inRange){
    if(Cast<AEnemy>(actor))
    Cast<AEnemy>(actor)->hitted(damage);
  }
  this->Destroy();*/
}

