// Fill out your copyright notice in the Description page of Project Settings.


#include "Room.h"

// Sets default values
ARoom::ARoom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARoom::BeginPlay()
{
	Super::BeginPlay();
	FVector minPoint = FVector(FMath::Min(point1.X,point2.X),FMath::Min(point1.Y,point2.Y),0);
	FVector maxPoint = FVector(FMath::Max(point1.X,point2.X),FMath::Max(point1.Y,point2.Y),0);
	FTransform trans;
	trans.SetLocation(minPoint);
	GetWorld()->SpawnActor<AActor>(wall,trans);
	trans.SetLocation(maxPoint);
	GetWorld()->SpawnActor<AActor>(wall,trans);
}

// Called every frame
void ARoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

