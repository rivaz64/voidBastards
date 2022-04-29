// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementState_Dodging.h"
#include "../Movement_C.h"
#include "Components/BoxComponent.h"

MovementState_Dodging::MovementState_Dodging(UMovement_C* movementComponent)
{
  MovementComponent = movementComponent;
}

void MovementState_Dodging::Init()
{
  MovementComponent->DodgingTime = 0.0f;
}

bool MovementState_Dodging::Update(float DeltaTime, FVector& outMovement)
{
  if (!MovementState::Update(DeltaTime, outMovement))
  {
    GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, "Dodging");

    MovementComponent->MovementVelocity += MovementComponent->DodgingAcceleration * DeltaTime;
    if (MovementComponent->MovementVelocity > MovementComponent->DodgingMaxVelocity)
    {
      MovementComponent->MovementVelocity = MovementComponent->DodgingMaxVelocity;
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


    MovementComponent->DodgingTime += DeltaTime;
    if (MovementComponent->DodgingTime > MovementComponent->DodgingMaxTime)
    {
      MovementComponent->Dodging = false;
    }
  }

  return true;
}

void MovementState_Dodging::Exit()
{
}
