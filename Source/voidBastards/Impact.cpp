// Fill out your copyright notice in the Description page of Project Settings.


#include "Impact.h"

// Sets default values
AImpact::AImpact()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AImpact::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AImpact::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

