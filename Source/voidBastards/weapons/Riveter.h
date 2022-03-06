// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FireArm.h"
#include "Riveter.generated.h"

/**
 * 
 */
UCLASS()
class VOIDBASTARDS_API URiveter : public UFireArm
{
	GENERATED_BODY()
	
 public:

	void
	shot() override;

	void
	shotting() override;

	void
	stopShotting() override;

	bool flipFloop = false;

};
