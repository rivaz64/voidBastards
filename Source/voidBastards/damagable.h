// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "damagable.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VOIDBASTARDS_API Udamagable : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	Udamagable();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void
	hitted(int damage);

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int vida;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int maxVida;

  UFUNCTION(BlueprintCallable)
	float 
	getVidaPercentage();

	
};