// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "FireArm.generated.h"

/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class VOIDBASTARDS_API UFireArm : public UWeapon
{
	GENERATED_BODY()

 public:

	void
	shot() override;
	
	virtual void
	fire(){}

	void
	bulletTrace(FVector dir, float distance);

	void
	empty() override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<UObject*> fireSprites;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<UObject*> emptySprites;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<TSubclassOf<AActor>> marks;
};
