// Fill out your copyright notice in the Description page of Project Settings.


#include "Spiker.h"
#include "../Enemy.h"
#include "../damagable.h"

void
USpiker::fire()
{
  bulletTrace(getDirection(),2000);
}

bool
USpiker::damageEnemy(const FHitResult& result){
  auto impact = result.ImpactPoint;
  
  auto hittedActor = result.GetActor();
  auto enemy = Cast<AEnemy>(hittedActor);
 
  if(enemy){
    
    Cast<Udamagable>(enemy->GetComponentByClass(Udamagable::StaticClass()))->poisoned(poisonDamage);
    return true;
  }
  return false;
}