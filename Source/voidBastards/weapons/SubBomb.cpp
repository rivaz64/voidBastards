// Fill out your copyright notice in the Description page of Project Settings.


#include "SubBomb.h"

void ASubBomb::BeginPlay(){
  Super::BeginPlay();
  timeToExplode = FMath::FRand()*9.f+1.f;
  
}

void ASubBomb::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
  Update();
}
