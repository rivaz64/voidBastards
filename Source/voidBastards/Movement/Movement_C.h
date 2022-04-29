// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Movement_C.generated.h"

class MovementState;
class MovementStateLink;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VOIDBASTARDS_API UMovement_C : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMovement_C();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



  /////////////////////////////////////////////////////////////////////////////
  /*                                                                         */
  /*                                Functions                                */
  /*                                                                         */
  /////////////////////////////////////////////////////////////////////////////

  void InitComponent(FVector _orientation, class UShapeComponent* _feetComponent);

  void Move(FVector direction);

  void Jump();
  void Dodge();
  void SetRunning(bool isRunning);

  FVector GetMovement();

  /////////////////////////////////////////////////////////////////////////////
  /*                                                                         */
  /*                               Components                                */
  /*                                                                         */
  /////////////////////////////////////////////////////////////////////////////

  class UShapeComponent* FeetComponent;


	/* Movement */

	FVector PlayerOrientation;
  FVector SumForces;
  UPROPERTY(EditAnywhere, Category = Movement)
	float Mass = 10.0f;

  UPROPERTY(EditAnywhere, Category = Movement)
	float MovementForceStrenght = 100.0f;


  UPROPERTY(EditAnywhere, Category = Movement)
	float MaxVelocity = 500.0f;
	float MaxVelocityDynamic = 500.0f;
  float MovementVelocity = 0.0f;
  UPROPERTY(EditAnywhere, Category = Movement)
  float MovementAcceleration = 600.0f;
  float MovementAccelerationDynamic = 600.0f;
  UPROPERTY(EditAnywhere, Category = Movement)
	float MovementDeacceleration = 1000.0f;
	float MovementDeaccelerationDynamic = 1000.0f;
	bool Moving = false;

  float YVelocity = 0.0f;
  UPROPERTY(EditAnywhere, Category = Movement)
	float Gravity = 980.0f;

  FVector4 FloorPlane;
  FVector4 CealingPlane;


  UPROPERTY(EditAnywhere, Category = Movement)
  float GravityForce = 980.0f;
  UPROPERTY(EditAnywhere, Category = Movement)
  float JumpForce = 500.0f;
  UPROPERTY(EditAnywhere, Category = Movement)
  float DoubleJumpForce = 400.0f;

  float VerticalAcceleration = 0.0f;
  bool OnGround = false;
  bool HasDoubleJump = true;


  bool Attacking = false;


  bool Dodging = false;
  float DodgingTime = 0.0f;
  UPROPERTY(EditAnywhere, Category = Movement)
  float DodgingMaxTime = 0.25f;
  UPROPERTY(EditAnywhere, Category = Movement)
  float DodgingCooldown = 0.6f;
  float DodgingCurrentCooldown = 10.0f;
  UPROPERTY(EditAnywhere, Category = Movement)
  float DodgingMaxVelocity = 5000.0f;
  UPROPERTY(EditAnywhere, Category = Movement)
  float DodgingAcceleration = 8000.0f;


  bool Running = false;
  UPROPERTY(EditAnywhere, Category = Movement)
  float RunningMaxVelocity = 1000.0f;
  UPROPERTY(EditAnywhere, Category = Movement)
  float RunningAcceleration = 1200.0f;



  FVector TotalMovement;



  /////////////////////////////////////////////////////////////////////////////
  /*                                                                         */
  /*                              State Machine                              */
  /*                                                                         */
  /////////////////////////////////////////////////////////////////////////////

  TMap<uint8, MovementState*> MachineStates;

  TArray<MovementStateLink*> Links;
};
