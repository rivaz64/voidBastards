// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Vision.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VOIDBASTARDS_API UVision : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVision();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float distance = 200;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float near = 1000;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float angle = .5;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float timer=.5f;

	float time;

	bool seeing;
		
};
