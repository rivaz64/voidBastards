// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FireArm.h"
#include "Stapler.generated.h"

/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class VOIDBASTARDS_API UStapler : public UFireArm
{
	GENERATED_BODY()

 public:

	void
	shot() override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int bulletsPerShot = 6;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float separation = .15;
};
