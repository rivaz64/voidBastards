// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Weapon.generated.h"

/**
 * 
 */

 class UPaperSprite;
UCLASS(Blueprintable,BlueprintType)
class VOIDBASTARDS_API UWeapon : public UObject
{
	GENERATED_BODY()
public:

	void 
	use();

	virtual void
	shot() {}

	virtual void
	empty(){}

	FVector
	getLocation();

	FVector
	getDirection();

	FVector
	getDirectionRight();

	FVector
	getDirectionUp();

	UWorld* world;

	ACharacter* character;

	
 
  UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<int> bulletDamages;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int bullets;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<UObject*> readySprites;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int level = 0;
};
