// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Indirect.generated.h"

/**
 * 
 */
 class AThrowable;
UCLASS(Blueprintable,BlueprintType)
class VOIDBASTARDS_API UIndirect : public UWeapon
{
	GENERATED_BODY()
	
 public:

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TSubclassOf<AActor> throwable;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float impulse;

	void 
	shot() override;
};
