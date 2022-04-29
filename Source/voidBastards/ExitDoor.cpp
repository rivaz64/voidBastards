// Fill out your copyright notice in the Description page of Project Settings.


#include "ExitDoor.h"

// Sets default values
AExitDoor::AExitDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AExitDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExitDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

