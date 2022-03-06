// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Janitor.generated.h"

/**
 * 
 */
UCLASS()
class VOIDBASTARDS_API AJanitor : public AEnemy
{
	GENERATED_BODY()
	
	/*void 
	attack() override;*/

	void
	attackUpdate() override;
};
