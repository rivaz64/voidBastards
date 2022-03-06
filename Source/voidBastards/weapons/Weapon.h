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
	shot();

	virtual void
	shotting() {}

	virtual void
	stopShotting(){}

	virtual void
	empty();

	virtual void
	fire(){}

	virtual bool
	damageEnemy(const FHitResult& result);

	void
	bulletTrace(FVector dir, float distance);

	void
	bulletSpawn(FVector dir);

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TSubclassOf<AActor> throwable;

	FVector
	getLocation();

	FVector
	getDirection();

	FVector
	getDirectionRight();

	FVector
	getDirectionUp();

	ACharacter* character;
 
  UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<int> bulletDamages;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int bullets;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<UObject*> readySprites;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<UObject*> fireSprites;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<UObject*> emptySprites;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int level = 0;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float separation = .15;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<TSubclassOf<AActor>> marks;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float impulse = 1000;

	float timer = 0;

	float delta;
};
