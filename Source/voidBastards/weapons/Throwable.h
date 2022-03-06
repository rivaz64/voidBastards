// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Throwable.generated.h"

UCLASS(Blueprintable,BlueprintType)
class VOIDBASTARDS_API AThrowable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThrowable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void 
	explote();

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float radius = 500;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float damage = 3;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	UStaticMeshComponent* mesh;

};
