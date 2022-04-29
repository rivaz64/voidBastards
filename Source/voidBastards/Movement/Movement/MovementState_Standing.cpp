// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementState_Standing.h"
#include "../Movement_C.h"
#include "Components/BoxComponent.h"

MovementState_Standing::MovementState_Standing(UMovement_C* movementComponent)
{
  MovementComponent = movementComponent;
}

void MovementState_Standing::Init()
{
}

bool MovementState_Standing::Update(float DeltaTime, FVector& outMovement)
{
  if (!MovementState::Update(DeltaTime, outMovement))
  {
    GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Blue, "Standing");
    
    MovementComponent->MovementVelocity -= MovementComponent->MovementDeacceleration * DeltaTime;
    if (MovementComponent->MovementVelocity < 0.0f)
    {
      MovementComponent->MovementVelocity = 0.0f;
    }
    
    if (MovementComponent->OnGround)
    {
      FVector newOrientation = MovementComponent->FeetComponent->GetComponentLocation() + MovementComponent->PlayerOrientation * 100.0f;
      newOrientation = FVector(newOrientation.X,
                               newOrientation.Y,
                              (MovementComponent->FloorPlane.W -
                               MovementComponent->FloorPlane.X * newOrientation.X -
                               MovementComponent->FloorPlane.Y * newOrientation.Y) /
                               MovementComponent->FloorPlane.Z);
      newOrientation -= MovementComponent->FeetComponent->GetComponentLocation();
      newOrientation.Normalize();
    
      outMovement += newOrientation * MovementComponent->MovementVelocity * DeltaTime;
    }
    else
    {
      outMovement += MovementComponent->PlayerOrientation * MovementComponent->MovementVelocity * DeltaTime;
    }
  }

  return true;
}

void MovementState_Standing::Exit()
{
}
