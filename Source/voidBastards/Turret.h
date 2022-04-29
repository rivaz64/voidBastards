// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Turret.generated.h"

UCLASS()
class VOIDBASTARDS_API ATurret : public AEnemy
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurret();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
  void activate();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void deactivate();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UStaticMeshComponent* rotable;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool activated = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float timer = .1f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float impulse = 1000.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float height = 100.f;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TSubclassOf<AActor> throwable;

	float time=0;
};
