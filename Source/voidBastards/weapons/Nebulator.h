// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FireArm.h"
#include "Nebulator.generated.h"

/**
 * 
 */
UCLASS()
class VOIDBASTARDS_API UNebulator : public UFireArm
{
	GENERATED_BODY()
 public:
	void
	fire() override;
};
