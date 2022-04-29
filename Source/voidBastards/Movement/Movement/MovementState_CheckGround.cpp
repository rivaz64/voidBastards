// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementState_CheckGround.h"
#include "../Movement_C.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include "../GlobalManagerInstance.h"

MovementState_CheckGround::MovementState_CheckGround(UMovement_C* movementComponent)
{
  MovementComponent = movementComponent;
}

void MovementState_CheckGround::Init()
{
}

bool MovementState_CheckGround::Update(float DeltaTime, FVector& outMovement)
{
  if (!MovementState::Update(DeltaTime, outMovement))
  {
    GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, "Checking Ground");

    auto manager =
    Cast<UGlobalManagerInstance>(UGameplayStatics::GetGameInstance(MovementComponent->GetOwner()));

    if (manager)
    {
      FVector DownPos;
      float DownDist;
      FVector4 DownPlane;
      FVector UpPos;
      float UpDist;
      FVector4 UpPlane;
      if (manager->GetCollisionsVertical
      (
        MovementComponent->FeetComponent->GetComponentLocation(),
        DownPos, DownDist, DownPlane,
        UpPos, UpDist, UpPlane
      ))
      {
        if (DownDist < 5.0f + abs(MovementComponent->VerticalAcceleration) * .02f)
        {
          if (MovementComponent->VerticalAcceleration <= 0.0f)
          {
            outMovement += FVector::DownVector * DownDist;
            MovementComponent->OnGround = true;
            MovementComponent->HasDoubleJump = true;
            MovementComponent->VerticalAcceleration = 0.0f;
            MovementComponent->FloorPlane = DownPlane;
          }
        }
        else if (UpDist < 5.0f + abs(MovementComponent->VerticalAcceleration) * .05f)
        {
          if (MovementComponent->VerticalAcceleration <= 0.0f)
          {
            outMovement += FVector::UpVector * UpDist;
            MovementComponent->OnGround = true;
            MovementComponent->HasDoubleJump = true;
            MovementComponent->VerticalAcceleration = 0.0f;
            MovementComponent->CealingPlane = UpPlane;
          }
        }
        else
        {
          MovementComponent->OnGround = false;
        }
      }
    }
  }

  return true;
}

void MovementState_CheckGround::Exit()
{
}
