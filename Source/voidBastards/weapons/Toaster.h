// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FireArm.h"
#include "Toaster.generated.h"

/**
 * 
 */
UCLASS()
class VOIDBASTARDS_API UToaster : public UFireArm
{
	GENERATED_BODY()
	
 public:
  
	void
	shot() override;

	void
	shotting() override;

	void
	stopShotting() override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<UObject*> loadingSprites;

	int energy = -1;
};
