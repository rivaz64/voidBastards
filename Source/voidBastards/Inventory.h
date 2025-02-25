// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include <vector>
#include <map>
#include "Inventory.generated.h"

class UPart;
class UWeapon;
class UFireArm;
class UIndirect;
class UDevice;

/**
 * 
 */

enum VB_PIEZAS{
P_METAL_TUBE = 0,
P_STIMULATOR,
P_FTL,
P_PLATE,
P_RIVET,
P_PNEUMA
};

UCLASS()
class VOIDBASTARDS_API UInventory : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	public:

	UInventory();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TMap<int,int> inventory;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TMap<int,FString> parts;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TMap<FString,bool> afordances;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TMap<FString,UWeapon*> weapons;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UFireArm* fireArm;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UIndirect* indirect;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UDevice* device;

	std::map<FString,std::vector<std::pair<int,int>>> prices;

	UFUNCTION( BlueprintCallable)
	void 
	addToInventory(UPart* part);

	UFUNCTION( BlueprintCallable)
	void 
	calculateAfordances();

	UFUNCTION( BlueprintCallable)
	void
	buy(FString name);

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool dirty = false;
};
