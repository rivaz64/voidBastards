// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Room.generated.h"

UCLASS()
class VOIDBASTARDS_API ARoom : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoom();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector point1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector point2;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FVector> doors;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<AActor> wall;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<AActor> door;

};
