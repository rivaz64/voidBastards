// Fill out your copyright notice in the Description page of Project Settings.


#include "Janitor.h"
#include "Steering.h"
#include "Projectile.h"
/*
void 
AJanitor::attack()
{
  /*FTransform trans;
	auto dir = FVector(steering->forward.X,steering->forward.Y,0);
	trans.SetLocation(GetActorLocation()+dir*100);
	auto projectil = GetWorld()->SpawnActor<AActor>(bullet,trans);
	Cast<AProjectile>(projectil)->mesh->AddImpulse(dir*1000);
}*/

void AJanitor::attackUpdate()
{/*
	actor->look();
	actor->timer += actor->DeltaTime;
	if(actor->timer > actor->timerAtack){
		anims->setWalkAnim();
		actor->timer = 0;
		actor->actualState = actor->patrolState;
	}*/
}
