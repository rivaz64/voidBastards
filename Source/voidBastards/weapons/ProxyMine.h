// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Throwable.h"
#include "ProxyMine.generated.h"

class AvoidBastardsCharacter;

/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class VOIDBASTARDS_API AProxyMine : public AThrowable
{
	GENERATED_BODY()

	public:

	UFUNCTION(BlueprintCallable)
	void
	enterRange(AActor* actor);

	UFUNCTION(BlueprintCallable)
	void
	exitRange(AActor* actor);

	UFUNCTION(BlueprintCallable)
	void 
	explode(AActor* actor);

	TArray<AActor*> inRange;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float damage = 6;

	AvoidBastardsCharacter* player;
	
};
