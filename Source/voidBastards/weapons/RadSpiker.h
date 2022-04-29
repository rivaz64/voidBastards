// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Indirect.h"
#include "RadSpiker.generated.h"

/**
 * 
 */
UCLASS()
class VOIDBASTARDS_API URadSpiker : public UIndirect
{
	GENERATED_BODY()
	 public:
	void
	fire() override;
};
