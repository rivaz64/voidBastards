// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "HarmonyController.generated.h"


namespace Harmony{
  class UnrealController;
}

UCLASS()
class VOIDBASTARDS_API AHarmonyController : public AAIController
{
	GENERATED_BODY()

 public:

  virtual void 
  OnPossess(APawn* InPawn) override;

  virtual void 
  Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere,Category = "wander")
  float wanderAceptanceRadius;

  UPROPERTY(EditAnywhere,Category = "wander")
  float wanderDelta;

  UPROPERTY(EditAnywhere,Category = "wander")
  float wanderRadius;

  UPROPERTY(EditAnywhere,Category = "pursue")
  float pursueRadius;

  Harmony::UnrealController* m_controller;
	
};
