// Fill out your copyright notice in the Description page of Project Settings.


#include "Bomb.h"

void 
ABomb::explote()
{
  FTransform trans;
  FVector dir;
  float angle;
  for(int i=0; i<numOfBombs;++i){
    angle = FMath::FRand()*PI*2.f;
    dir.X=cos(angle);
    dir.Y=sin(angle);
    dir.Z = 0;
    trans.SetLocation(GetActorLocation()+dir*100);
    FActorSpawnParameters SpawnInfo;
    SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnInfo.bNoFail = true;
    auto object = GetWorld()->SpawnActor<AThrowable>(throwable,trans,SpawnInfo);
    dir.Z = FMath::FRand()*(maxAtitude-minAtitude)+minAtitude;
    object->mesh->AddImpulse(dir*impulse);
  }
  this->Destroy();
}
