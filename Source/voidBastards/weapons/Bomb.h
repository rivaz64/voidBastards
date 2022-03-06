// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Throwable.h"
#include "Bomb.generated.h"

/**
 * 
 */
UCLASS()
class VOIDBASTARDS_API ABomb : public AThrowable
{
	GENERATED_BODY()
	
 public:

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TSubclassOf<AActor> throwable;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int numOfBombs = 6;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float impulse = 3600;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float maxAtitude;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float minAtitude;

	void 
	explote() override;
};
