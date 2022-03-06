// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Indirect.h"
#include "Spiker.generated.h"

/**
 * 
 */
UCLASS()
class VOIDBASTARDS_API USpiker : public UIndirect
{
	GENERATED_BODY()
	
 public:

  void
	fire() override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int poisonDamage = 1;

	bool
	damageEnemy(const FHitResult& result) override;
};
