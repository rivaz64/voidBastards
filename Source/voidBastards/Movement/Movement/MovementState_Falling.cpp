// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementState_Falling.h"
#include "../Movement_C.h"

MovementState_Falling::MovementState_Falling(UMovement_C* movementComponent)
{
  MovementComponent = movementComponent;
}

void MovementState_Falling::Init()
{
}

bool MovementState_Falling::Update(float DeltaTime, FVector& outMovement)
{
  if (!MovementState::Update(DeltaTime, outMovement))
  {
    GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, "Falling");

    MovementComponent->VerticalAcceleration -= MovementComponent->GravityForce * DeltaTime;
    outMovement += FVector::UpVector * MovementComponent->VerticalAcceleration * DeltaTime;
  }

  return true;
}

void MovementState_Falling::Exit()
{
}
