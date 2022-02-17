// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Part.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom),Blueprintable)
class VOIDBASTARDS_API UPart : public UObject
{
	GENERATED_BODY()
	public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UObject* image;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString name;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int id;
};

