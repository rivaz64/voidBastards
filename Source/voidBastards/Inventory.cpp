// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"
#include "Part.h"

UInventory::UInventory(){
	prices = {
	{FString("heavyRegulator"),{
	{P_METAL_TUBE,1},}
	},

	{FString("Fastwhaker"),{
	{P_METAL_TUBE,1},
	{P_STIMULATOR,1},}
	}

	};
	afordances.Add(FString("heavyRegulator"),false);
}

void UInventory::addToInventory(UPart* part)
{
	if(!part) return;
	if(!inventory.Contains(part->id)){
		inventory.Add(part->id,0);
		parts.Add(part->id,part->name);
	}
	++inventory[part->id];
}

void UInventory::calculateAfordances()
{
	TArray<FString> keys;
	afordances.GetKeys(keys);
	for(FString& key : keys){
		auto& price = prices[key];
		afordances[key] = true;;
		for(std::pair<int,int>& subPrice:price){
			auto w = inventory.Find(subPrice.first);
			if(!w || (*w)<subPrice.second){
				afordances[key] = false;
			}
		}
	}
}

void UInventory::buy(FString name)
{
	auto& price = prices[name];
	std::vector<int> paraEliminar;
	for(std::pair<int,int>& subPrice:price){
		inventory[subPrice.first]-=subPrice.second;
		if(inventory[subPrice.first]==0){
			paraEliminar.push_back(subPrice.first);
		}
	}
	for(int i:paraEliminar){
		parts.Remove(i);
		inventory.Remove(i);
	}
	calculateAfordances();
}
