// Fill out your copyright notice in the Description page of Project Settings.


#include "Steering.h"
#include "Enemy.h"
#include "Animation.h"
#include "Vision.h"
#include "Projectile.h"

// Sets default values for this component's properties
USteering::USteering()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	patrolState = new PatrolState;
	attackState = new AttackingState;
	chasingState = new ChasingState;
	// ...
}


// Called when the game starts
void USteering::BeginPlay()
{
	Super::BeginPlay();
	actualState = patrolState;
	// ...
	
}


// Called every frame
void 
USteering::TickComponent(float deltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DeltaTime = deltaTime;
	if(DeltaTime>=1.f/60.f){
		DeltaTime = 1.f/60.f;
	}

	auto pawn = Cast<AEnemy>(GetOwner());

	location = FVector2D(pawn->GetActorLocation().X,pawn->GetActorLocation().Y);

	distance = pointToGo-location;

	actualState->onUpdate(this);
}

void USteering::move()
{
	auto pawn = Cast<AEnemy>(GetOwner());

	FVector2D desiredVelocity = distance.GetSafeNormal()*maxVelocity;

	float arriveDist = (distance.Size()-aceptanceRadius)/(arrivingRadius-aceptanceRadius);

	if(distance.Size()<aceptanceRadius || arriveDist < .05){
		desiredVelocity *=0;
		timer+=DeltaTime;
		actualState->onArrive(this);
	}
	else if(distance.Size()<arrivingRadius){
		desiredVelocity *= arriveDist;
		timer = 0;
	}
	else{
		timer = 0;
	}

	aceleration = (desiredVelocity - velocity)*acelerationMultiplier;

	if(aceleration.Size() > maxAceleration){
		aceleration = aceleration.GetSafeNormal()*maxAceleration;
	}

	velocity += aceleration*maxAceleration*DeltaTime;

	if(velocity.Size() > maxVelocity){
		velocity = velocity.GetSafeNormal()*maxVelocity;
	}

	pawn->AddActorWorldOffset(FVector(DeltaTime*velocity.X,DeltaTime*velocity.Y,0),true);

	if(velocity.Size()>1){
		forward = velocity.GetSafeNormal();
		right = FVector2D(forward.Y,-forward.X);
	}
	forward = forward.GetSafeNormal();
	right = FVector2D(forward.Y,-forward.X);
}

void USteering::look()
{
	forward = distance.GetSafeNormal();
	right = FVector2D(forward.Y,-forward.X);
	velocity = forward;
}

void PatrolState::onArrive(USteering* actor)
{
	if(actor->timer > actor->waitTime){
		actor->acutalPoint = (actor->acutalPoint+1)%actor->pointsToGo.Num();
		actor->pointToGo = actor->pointsToGo[actor->acutalPoint];
	}
	
}

void ChasingState::onArrive(USteering* actor)
{
	auto pawn = Cast<AEnemy>(actor->GetOwner());
	if(!pawn->vision->seeing){
		if(actor->timer > actor->waitTime){
			actor->actualState = actor->patrolState;
			actor->pointToGo = actor->pointsToGo[actor->acutalPoint];
		}
	}
	else{
		actor->actualState = actor->attackState;
	  pawn->anims->setAttackAnim();
	}
	
}

void BehaviorState::onUpdate(USteering* actor)
{
	actor->move();
}

void BehaviorState::onSeen(USteering* actor)
{
	auto pawn = Cast<AEnemy>(actor->GetOwner());
	pawn->anims->setAlertAnim();
	actor->actualState = actor->chasingState;
}

void AttackingState::onUpdate(USteering* actor)
{
	auto pawn = actor->GetOwner();
	actor->look();
	if(actor->timer > actor->waitTime){
		actor->timer = 0;
		FTransform trans;
		trans.SetLocation(pawn->GetActorLocation()+FVector(actor->forward.X*50.f,actor->forward.Y*50.f,0));
		//auto projectil = actor->GetWorld()->SpawnActor<AActor*>(AProjectile::StaticClass(),trans);
		
	}
}
