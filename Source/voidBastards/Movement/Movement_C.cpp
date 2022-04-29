// Fill out your copyright notice in the Description page of Project Settings.


#include "Movement_C.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

//#include "../../Ambient/GlobalCollisionManager.h"
#include "GlobalManagerInstance.h"

#include "Movement/MovementState.h"
#include "Movement/MovementState_Dodging.h"
#include "Movement/MovementState_Falling.h"
#include "Movement/MovementState_Jumping.h"
#include "Movement/MovementState_Standing.h"
#include "Movement/MovementState_Walking.h"
#include "Movement/MovementState_Running.h"
#include "Movement/MovementState_CheckGround.h"

#include "Movement/MovementStateLink.h"

// Sets default values for this component's properties
UMovement_C::UMovement_C()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMovement_C::BeginPlay()
{
	Super::BeginPlay();

  // ...

  MaxVelocityDynamic = MaxVelocity;
  MovementAccelerationDynamic = MovementAcceleration;	

  for (int32 i = 0; i < eSTATE_TYPE::kSTATE_COUNT; ++i)
  {
    switch (eSTATE_TYPE(i))
    {
    case kSTANDING:
      MachineStates.Add(TTuple<eSTATE_TYPE, MovementState*>(kSTANDING, new MovementState_Standing(this)));
      break;
    case kWALKING:
      MachineStates.Add(TTuple<eSTATE_TYPE, MovementState*>(kWALKING, new MovementState_Walking(this)));
      break;
    case kRUNNING:
      MachineStates.Add(TTuple<eSTATE_TYPE, MovementState*>(kRUNNING, new MovementState_Running(this)));
      break;
    case kJUMPING:
      MachineStates.Add(TTuple<eSTATE_TYPE, MovementState*>(kJUMPING, new MovementState_Jumping(this)));
      break;
    case kFALLING:
      MachineStates.Add(TTuple<eSTATE_TYPE, MovementState*>(kFALLING, new MovementState_Falling(this)));
      break;
    case kDODGING:
      MachineStates.Add(TTuple<eSTATE_TYPE, MovementState*>(kDODGING, new MovementState_Dodging(this)));
      break;
    case kCHECKING_GROUND:
      MachineStates.Add(TTuple<eSTATE_TYPE, MovementState*>(kCHECKING_GROUND, new MovementState_CheckGround(this)));
      break;
    }
  }

  Links.Add
  (
    new MovementStateLink
    (
      {
        MachineStates[kCHECKING_GROUND]
      }
    )
  );

  Links.Add
  (
    new MovementStateLink
    (
      {
        MachineStates[kSTANDING],
        new MovementStateLink
        (
          {
            new MovementStateLink
            (
              {
                MachineStates[kJUMPING]
              },
              {
                MachineStates[kFALLING]
              },
              reinterpret_cast<void*>(&VerticalAcceleration),
              eVALUE_TYPE::kFLOAT,
              reinterpret_cast<void*>(new float(0.0f)),
              eVALUE_TYPE::kFLOAT,
              eOPERATOR_TYPE::kGREATER
            )
          },
          {},
          reinterpret_cast<void*>(&OnGround), 
          eVALUE_TYPE::kBOOL,
          reinterpret_cast<void*>(new bool(false)),
          eVALUE_TYPE::kBOOL,
          eOPERATOR_TYPE::kEQUAL
        )
      },
      {
        MachineStates[kDODGING],
        new MovementStateLink
        (
          {
            new MovementStateLink
            (
              {
                MachineStates[kJUMPING]
              },
              {
                MachineStates[kFALLING]
              },
              reinterpret_cast<void*>(&VerticalAcceleration),
              eVALUE_TYPE::kFLOAT,
              reinterpret_cast<void*>(new float(0.0f)),
              eVALUE_TYPE::kFLOAT,
              eOPERATOR_TYPE::kGREATER
            )
          },
          {},
          reinterpret_cast<void*>(&OnGround),
          eVALUE_TYPE::kBOOL,
          reinterpret_cast<void*>(new bool(false)),
          eVALUE_TYPE::kBOOL,
          eOPERATOR_TYPE::kEQUAL
        )
      },
      reinterpret_cast<void*>(&Dodging),
      eVALUE_TYPE::kBOOL,
      reinterpret_cast<void*>(new bool(false)),
      eVALUE_TYPE::kBOOL,
      eOPERATOR_TYPE::kEQUAL
    )
  );

  MachineStates[kSTANDING]->AddLink
  (
    new MovementStateLink
    (
      {
        MachineStates[kWALKING]
      },
      {},
      reinterpret_cast<void*>(&SumForces),
      eVALUE_TYPE::kVECTOR,
      reinterpret_cast<void*>(new FVector(0.0f, 0.0f, 0.0f)),
      eVALUE_TYPE::kVECTOR,
      eOPERATOR_TYPE::kNOT_EQUAL
    )
  );

  MachineStates[kWALKING]->AddLink
  (
    new MovementStateLink
    (
      {
        MachineStates[kRUNNING]
      },
      {},
      reinterpret_cast<void*>(&Running),
      eVALUE_TYPE::kBOOL,
      reinterpret_cast<void*>(new bool(true)),
      eVALUE_TYPE::kBOOL,
      eOPERATOR_TYPE::kEQUAL
    )
  );
}


// Called every frame
void UMovement_C::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  for (auto l : Links)
  {
    l->CheckExit();
  }

  TotalMovement = FVector(0.0f, 0.0f, 0.0f);
  for (auto l : Links)
  {
    FVector mov = FVector(0.0f, 0.0f, 0.0f);
    if (l->Update(DeltaTime, mov))
    {
      TotalMovement += mov;
    }
  }
  GetOwner()->SetActorLocation(GetOwner()->GetActorLocation() + TotalMovement);

  SumForces = FVector(0.0f, 0.0f, 0.0f);
  DodgingCurrentCooldown += DeltaTime;
}

void UMovement_C::InitComponent(FVector _orientation, class UShapeComponent* _feetComponent)
{
  PlayerOrientation = _orientation;
  FeetComponent = _feetComponent;
}

void UMovement_C::Move(FVector direction)
{
  SumForces += direction;
}

void UMovement_C::Jump()
{
  if (!Dodging)
  {
    if (OnGround)
    {
      VerticalAcceleration += JumpForce;
      OnGround = false;
    }
    else if (HasDoubleJump)
    {
      VerticalAcceleration = DoubleJumpForce;
      HasDoubleJump = false;
    }
  }
}

void UMovement_C::Dodge()
{
  if (DodgingCurrentCooldown > DodgingCooldown && Moving)
  {
    Dodging = true;
    DodgingCurrentCooldown = 0.0f;
  }
}

void UMovement_C::SetRunning(bool isRunning)
{
  Running = isRunning;
}

FVector UMovement_C::GetMovement()
{
  return TotalMovement;
}

