// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Throwable.h"
#include "SubBomb.generated.h"

/**
 * 
 */
UCLASS()
class VOIDBASTARDS_API ASubBomb : public AThrowable
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	public:
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Update();
};
