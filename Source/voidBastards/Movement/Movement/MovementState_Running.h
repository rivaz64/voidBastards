// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovementState.h"

/**
 *
 */
class VOIDBASTARDS_API MovementState_Running : public MovementState
{
public:
  explicit MovementState_Running(UMovement_C* movementComponent);
  ~MovementState_Running() = default;

  void Init() override;
  bool Update(float DeltaTime, FVector& outMovement) override;
  void Exit() override;
};
