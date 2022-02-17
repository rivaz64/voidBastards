// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FireArm.h"
#include "Regulator.generated.h"

/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class VOIDBASTARDS_API URegulator : public UFireArm
{
	GENERATED_BODY()
	
 public:
  void
	fire() override;
};
