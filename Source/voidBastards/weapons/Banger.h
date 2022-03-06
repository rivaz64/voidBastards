// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Indirect.h"
#include "Banger.generated.h"

/**
 * 
 */
UCLASS()
class VOIDBASTARDS_API UBanger : public UIndirect
{
	GENERATED_BODY()
	
 public:

  void
	shot() override;

	void
	empty() override {}

	void
	fire() override;
};
