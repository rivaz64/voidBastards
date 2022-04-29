// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Steering.generated.h"

class BehaviorState;
class PatrolState;
class AttackingState;
class ChasingState;
class FleeingState;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VOIDBASTARDS_API USteering : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USteering();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void move();

	void look();

	float DeltaTime;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float maxVelocity = 200;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float acelerationMultiplier = 1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float maxAceleration = 100;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float patrolAceptanceRadius = 50;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float chasingAceptanceRadius = 300;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float timerAtack = 3;

	FVector2D location;

	FVector2D distance;

	FVector2D velocity;

	FVector2D aceleration;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector2D pointToGo;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector2D forward = FVector2D(1,0);

		UPROPERTY(EditAnywhere,BlueprintReadWrite)

	FVector2D right = FVector2D(0,1);

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FVector2D> pointsToGo;

	int acutalPoint = 0;

	float timer = 0;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float waitTime = 2;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float attackTime = .5;

	FVector2D memory;

	BehaviorState* actualState;
	PatrolState* patrolState;
	AttackingState* attackState;
	ChasingState* chasingState;
	FleeingState* fleeState;
};


class BehaviorState{
 public:
  virtual void onArrive(USteering* actor){}
	virtual void onUpdate(USteering* actor);
	virtual void onSeen(USteering* actor,const FVector2D& position);

	float aceptanceRadius;
};

class PatrolState: public BehaviorState{
	void onArrive(USteering* actor)override;
};

class ChasingState: public BehaviorState{
  void onArrive(USteering* actor)override;
};

class AttackingState: public BehaviorState{
  void onUpdate(USteering* actor)override;
	void onSeen(USteering* actor,const FVector2D& position)override{}
	int attacks = 3;
	int actualAttacks = 0;
};

class FleeingState: public BehaviorState{
	void onSeen(USteering* actor,const FVector2D& position)override{}
	void onArrive(USteering* actor)override;
};