// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class VOIDBASTARDS_API UMyCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	public:
	//virtual bool IsFlying() const override;
	
	bool isFlying = false;
};
