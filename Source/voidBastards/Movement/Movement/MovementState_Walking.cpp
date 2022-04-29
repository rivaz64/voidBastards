// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementState_Walking.h"
#include "../Movement_C.h"
#include "Components/BoxComponent.h"


MovementState_Walking::MovementState_Walking(UMovement_C* movementComponent)
{
  MovementComponent = movementComponent;
}

void MovementState_Walking::Init()
{
  MovementComponent->Moving = true;
}

bool MovementState_Walking::Update(float DeltaTime, FVector& outMovement)
{
  if (!MovementState::Update(DeltaTime, outMovement))
  {
    GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Magenta, "Walking");

    MovementComponent->SumForces *= MovementComponent->MovementForceStrenght * DeltaTime;
    MovementComponent->SumForces.Normalize();
    MovementComponent->PlayerOrientation += MovementComponent->SumForces * MovementComponent->Mass * DeltaTime;
    MovementComponent->PlayerOrientation.Normalize();
    
    MovementComponent->MovementVelocity += MovementComponent->MovementAccelerationDynamic * DeltaTime;
    if (MovementComponent->MovementVelocity > MovementComponent->MaxVelocityDynamic)
    {
      MovementComponent->MovementVelocity = MovementComponent->MaxVelocityDynamic;
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

void MovementState_Walking::Exit()
{
  MovementComponent->Moving = false;
}
