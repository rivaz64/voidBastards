// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovementState.h"

/**
 * 
 */
class VOIDBASTARDS_API MovementState_Falling : public MovementState
{
public:
  explicit MovementState_Falling(UMovement_C* movementComponent);
  ~MovementState_Falling() = default;

  void Init() override;
  bool Update(float DeltaTime, FVector& outMovement) override;
  void Exit() override;
};