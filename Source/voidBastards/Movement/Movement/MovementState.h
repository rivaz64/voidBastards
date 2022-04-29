// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class UMovement_C;
class MovementStateLink;

enum eSTATE_TYPE : uint8
{
  kSTATE_NONE,
  kSTANDING,
  kWALKING,
  kRUNNING,
  kJUMPING,
  kFALLING,
  kDODGING,
  kCHECKING_GROUND,
  kSTATE_COUNT
};

/**
 * 
 */
class VOIDBASTARDS_API MovementState
{
public:
	MovementState() = default;
  virtual ~MovementState() = default;

  virtual void Init() {}
  virtual bool Update(float DeltaTime, FVector& outMovement);
  virtual void Exit() {}

  virtual bool CheckExit();
  virtual void TryExit();

  virtual void AddLink(MovementStateLink* newLink);

 protected:
  bool FirstUpdate = true;

  UMovement_C* MovementComponent = nullptr;

  TArray<MovementStateLink*> Links;
};
