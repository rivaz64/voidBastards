// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class VOIDBASTARDS_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
 public:

	UPROPERTY(BlueprintReadWrite)
	bool isFire = false;

	UPROPERTY(BlueprintReadWrite)
	bool isEmpty = false;

	float timer;

	void 
	fire();

	UFUNCTION(BlueprintCallable)
	void update(float deltaTime);

};
