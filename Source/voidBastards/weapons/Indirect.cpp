// Fill out your copyright notice in the Description page of Project Settings.


#include "Indirect.h"
#include "Throwable.h"
//#include "ProxyMine.h"

void 
UIndirect::shot(){
  FTransform trans;
  auto dir = getDirection();
  trans.SetLocation(getLocation()+dir*250);
  FActorSpawnParameters SpawnInfo;
  SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
  SpawnInfo.bNoFail = true;
  auto object = GetWorld()->SpawnActor<AThrowable>(throwable,trans,SpawnInfo);

  //if(object)
  //Cast<AThrowable>(object)->mesh->AddImpulse(dir);
}