// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VisualCollicion.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VOIDBASTARDS_API UVisualCollicion : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVisualCollicion();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FVector feets;

	float upVel;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float gravity;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float collicionDistance;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float radialColicion;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int rounds;

	bool isInFloor;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float minFloorDist;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float radialOrigin=50;

	bool isColliding;

	FVector collidingVector;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool debugLine = false;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool debugFloor= false;
		
};
