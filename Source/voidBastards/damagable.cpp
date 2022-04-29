// Fill out your copyright notice in the Description page of Project Settings.


#include "damagable.h"
#include "Enemy.h"
#include "voidBastardsCharacter.h"
// Sets default values for this component's properties
Udamagable::Udamagable()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void Udamagable::BeginPlay()
{
	Super::BeginPlay();
	vida = maxVida;
	// ...
	
}


// Called every frame
void Udamagable::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	timer+=DeltaTime;

	if(timer > venomUpdate){
		timer = 0;
		if(venom>0)
		hitted(venom);
		if(radiationPercentage>0){
			hitted(radiationPercentage*radiationDamage);
		}
		//if(venom>0){
		//	auto 
		//}
	}
	// ...
}

void Udamagable::hitted(int damage)
{
	vida -= damage;
	auto enemy = Cast<AEnemy>(GetOwner());
	if(enemy){
		enemy->hitted();
	}
	if(vida<=0){
		if(enemy){
			enemy->die();
		}
		auto player = Cast<AvoidBastardsCharacter>(GetOwner());
		if(player){
			player->die();
		}
	}
}

float 
Udamagable::getVidaPercentage()
{
	return static_cast<float>(vida)/static_cast<float>(maxVida);
}

void 
Udamagable::poisoned(int poison)
{
	venom = poison;
	auto enemy = Cast<AEnemy>(GetOwner());
	if(enemy){
		enemy->onPoised();
	}
}

void 
Udamagable::radiated()
{
	radiationPercentage+=.01;
}
