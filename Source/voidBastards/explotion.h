// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "explotion.generated.h"

UCLASS()
class VOIDBASTARDS_API Aexplotion : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Aexplotion();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the game starts or when spawned
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float radius = 500;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float damage = 3;

};
