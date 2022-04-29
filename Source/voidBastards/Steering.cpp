// Fill out your copyright notice in the Description page of Project Settings.


#include "Steering.h"
#include "Enemy.h"
#include "Animation.h"
#include "Vision.h"
#include "Projectile.h"
#include "VisualCollicion.h"

// Sets default values for this component's properties
USteering::USteering()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	patrolState = new PatrolState;
	attackState = new AttackingState;
	chasingState = new ChasingState;
	fleeState = new FleeingState;
	patrolState->aceptanceRadius = patrolAceptanceRadius;
	fleeState->aceptanceRadius = patrolAceptanceRadius;
	chasingState->aceptanceRadius = chasingAceptanceRadius;
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
	
	//if(velocity.Size()<1){
	//	actualState->onArrive(this);
	//	//return;
	//}

	actualState->onUpdate(this);
}

void USteering::move()
{
	auto pawn = Cast<AEnemy>(GetOwner());

	FVector2D desiredVelocity = distance.GetSafeNormal()*maxVelocity;

	if(distance.Size()<actualState->aceptanceRadius ){
		desiredVelocity *=0;
		timer+=DeltaTime;
		actualState->onArrive(this);
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

	auto col =Cast<UVisualCollicion>(pawn->GetComponentByClass(UVisualCollicion::StaticClass()));
	if(col){
		if(col->isColliding && FVector2D::DotProduct(velocity,FVector2D(col->collidingVector.X,col->collidingVector.Y))){
			velocity = FVector2D::ZeroVector;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("colliding"));

		}
		
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%f %f"), velocity.X, velocity.Y));

	
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

//void USteering::flee()
//{
//}

void PatrolState::onArrive(USteering* actor)
{
	if(actor->timer > actor->waitTime){
		if(actor->pointsToGo.Num()==0) return;
		actor->acutalPoint = (actor->acutalPoint+1)%actor->pointsToGo.Num();
		actor->pointToGo = actor->pointsToGo[actor->acutalPoint];
	}
	
}

void ChasingState::onArrive(USteering* actor)
{
	auto pawn = Cast<AEnemy>(actor->GetOwner());
	auto anims = Cast<UAnimation>(pawn->GetComponentByClass(UAnimation::StaticClass()));
	auto vision = Cast<UVision>(pawn->GetComponentByClass(UVision::StaticClass()));
	if(!vision->seeing){

		if(actor->timer > actor->waitTime){

			anims->setWalkAnim();
			actor->actualState = actor->patrolState;
			actor->pointToGo = actor->pointsToGo[actor->acutalPoint];
		}
	}
	else{
		actor->actualState = actor->attackState;
	  anims->setAttackAnim();
	}
	
}

void BehaviorState::onUpdate(USteering* actor)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("None"));
	actor->move();
}

void BehaviorState::onSeen(USteering* actor,const FVector2D& position)
{
	auto pawn = Cast<AEnemy>(actor->GetOwner());
	auto anims = Cast<UAnimation>(pawn->GetComponentByClass(UAnimation::StaticClass()));
	actor->pointToGo = position;
	actor->memory = position;
	anims->setAlertAnim();
	actor->actualState = actor->chasingState;
}

void AttackingState::onUpdate(USteering* actor)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Attack"));

	auto pawn = Cast<AEnemy>(actor->GetOwner());
	actor->look();
	actor->timer += actor->DeltaTime;
	if(actor->timer > actor->timerAtack){
		auto anims = Cast<UAnimation>(pawn->GetComponentByClass(UAnimation::StaticClass()));

		anims->setWalkAnim();
		actor->timer = 0;
		actor->actualState = actor->patrolState;
	}
}

void FleeingState::onArrive(USteering* actor)
{	
	actor->actualState = actor->patrolState;
	actor->actualState->onArrive(actor);
	actor->pointToGo = actor->memory;
}
