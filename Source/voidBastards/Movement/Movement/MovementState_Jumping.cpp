// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementState_Jumping.h"
#include "../Movement_C.h"

MovementState_Jumping::MovementState_Jumping(UMovement_C* movementComponent)
{
  MovementComponent = movementComponent;
}

void MovementState_Jumping::Init()
{
}

bool MovementState_Jumping::Update(float DeltaTime, FVector& outMovement)
{
  if (!MovementState::Update(DeltaTime, outMovement))
  {
    GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, "Jumping");
    
    MovementComponent->VerticalAcceleration -= MovementComponent->GravityForce * DeltaTime;
    outMovement += FVector::UpVector * MovementComponent->VerticalAcceleration * DeltaTime;
  }

  return true;
}

void MovementState_Jumping::Exit()
{
}
