// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Explotable.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) ,Blueprintable,BlueprintType)
class VOIDBASTARDS_API UExplotable : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UExplotable();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void
	enterRange(AActor* actor);

	UFUNCTION(BlueprintCallable)
	void
	exitRange(AActor* actor);

	UFUNCTION(BlueprintCallable)
	void 
	explode(AActor* actor);

	

	TArray<AActor*> inRange;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float damage = 6;
		
};
