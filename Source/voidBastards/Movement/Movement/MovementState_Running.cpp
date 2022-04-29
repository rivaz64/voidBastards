// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementState_Running.h"
#include "../Movement_C.h"
#include "Components/BoxComponent.h"

MovementState_Running::MovementState_Running(UMovement_C* movementComponent)
{
  MovementComponent = movementComponent;
}

void MovementState_Running::Init()
{
  MovementComponent->Moving = true;
}

bool MovementState_Running::Update(float DeltaTime, FVector& outMovement)
{
  if (!MovementState::Update(DeltaTime, outMovement))
  {
    GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Black, "Running");

    MovementComponent->SumForces *= MovementComponent->MovementForceStrenght * DeltaTime;
    MovementComponent->SumForces.Normalize();
    MovementComponent->PlayerOrientation += MovementComponent->SumForces * MovementComponent->Mass * DeltaTime;
    MovementComponent->PlayerOrientation.Normalize();

    MovementComponent->MovementVelocity += MovementComponent->RunningAcceleration * DeltaTime;
    if (MovementComponent->MovementVelocity > MovementComponent->RunningMaxVelocity)
    {
      MovementComponent->MovementVelocity = MovementComponent->RunningMaxVelocity;
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

void MovementState_Running::Exit()
{
  MovementComponent->Moving = false;
}
