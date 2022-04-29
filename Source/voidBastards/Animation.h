// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation.generated.h"

namespace ANIMATION_ENUM{
	enum class E{
		DIE,
		IDLE,
		WALK
	};
}

USTRUCT(BlueprintType)
struct FAnim
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<UObject*> frames;
};

class AnimState;
class IdleAnim;
class WalkAnim;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VOIDBASTARDS_API UAnimation : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAnimation();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void
	setDeadAnim();

	void
	setIdleAnim();

	void
	setWalkAnim();

	void
	setAlertAnim();

	void
	setAttackAnim();

	void
	setEnrageAnim();

	void
	setDirection(const FVector2D& dir);

	protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FAnim> idleAnim;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FAnim> deadAnim;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FAnim> walkAnim;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FAnim> alertAnim;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FAnim> attackAnim;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FAnim> enrageAnim;
	
	TArray<FAnim>* actualGroup;

  TArray<UObject*>* frames;

  UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool inverted;

	int actualAnim;

	float timer=0;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float secondPerFrame = .5;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UObject* sprite;

	AnimState* actualState;
	AnimState* noState;
	IdleAnim* idleState;
	WalkAnim* walkState;


};

class AnimState{
 public:
	virtual void onUpdate(UAnimation* anim);
};

class IdleAnim : public AnimState
{
	void onUpdate(UAnimation* anim);
};

class WalkAnim : public AnimState
{
	void onUpdate(UAnimation* anim);
};
