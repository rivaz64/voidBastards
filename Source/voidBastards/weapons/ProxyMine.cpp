// Fill out your copyright notice in the Description page of Project Settings.


#include "ProxyMine.h"
#include "../Enemy.h"
#include "../voidBastardsCharacter.h"
#include "../damagable.h"

void
AProxyMine::enterRange(AActor* actor){
  inRange.Add(actor);
}

void AProxyMine::exitRange(AActor* actor)
{
  inRange.Remove(actor);
}

void AProxyMine::explode(AActor* actor)
{
  if(Cast<AProxyMine>(actor)) return;
  player = Cast<AvoidBastardsCharacter>(actor);
  if(player) return;
  GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("exploted")));
  for(AActor* actor:inRange){
    auto damagable = actor->GetComponentsByClass(Udamagable::StaticClass());//->GetComponentsByClass<Udamagable>();
    if(damagable.Num()>0){
      Cast<Udamagable>(damagable[0])->hitted(damage);
    }
  }
  this->Destroy();
}
