// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovementState.h"

/**
 * 
 */
class VOIDBASTARDS_API MovementState_Jumping : public MovementState
{
public:
  explicit MovementState_Jumping(UMovement_C* movementComponent);
  ~MovementState_Jumping() = default;

  void Init() override;
  bool Update(float DeltaTime, FVector& outMovement) override;
  void Exit() override;
};
