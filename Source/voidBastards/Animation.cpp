// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation.h"
#include "Enemy.h"
#include "Steering.h"
// Sets default values for this component's properties
UAnimation::UAnimation()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	idleState = new IdleAnim;
	walkState = new WalkAnim;
	noState = new AnimState;
	// ...
}


// Called when the game starts
void UAnimation::BeginPlay()
{
	Super::BeginPlay();

	// ;
	setIdleAnim();
	frames = &idleAnim[0].frames;
}


// Called every frame
void UAnimation::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	timer += DeltaTime;

	auto pawn = Cast<AEnemy>(GetOwner());

	if(timer>secondPerFrame){
		++actualAnim;
		auto& actual = *frames;
		sprite = actual[actualAnim%actual.Num()];
		pawn->printSprite();
		timer = 0;
	}

	actualState->onUpdate(this);
}

void UAnimation::setDeadAnim()
{
	actualGroup = &deadAnim;
	actualState = noState;
}

void UAnimation::setIdleAnim()
{
  actualGroup = &idleAnim;
	actualState = idleState;
}

void UAnimation::setWalkAnim()
{
	if(walkAnim.Num()!=0){
		actualGroup = &walkAnim;
	}
	actualState = walkState;
}

void UAnimation::setAlertAnim()
{
	if(alertAnim.Num()!=0)
	actualGroup = &alertAnim;
	actualState = noState;
}

void UAnimation::setAttackAnim()
{
	if(attackAnim.Num()!=0)
	actualGroup = &attackAnim;
}

void 
UAnimation::setDirection(const FVector2D& dir)
{
	//if(actualGroup == &walkAnim) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("walking"));
	//if(actualGroup == &idleAnim) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("idleing"));

	auto& actual = *actualGroup;
	auto pawn = Cast<AEnemy>(GetOwner());
	if(!pawn->steering) return;
	float right = FVector2D::DotProduct(dir,pawn->steering->right);
	float forward = FVector2D::DotProduct(dir,pawn->steering->forward);
	//if(!actual.size) return;
	if(abs(right)>0.92387953251 || abs(forward)>0.92387953251){
		if(abs(right)>abs(forward)){
			frames = &actual[2].frames;
		}
		else{
			if(forward>0){
				frames = &actual[0].frames;
			}
			else{
				frames = &actual[1].frames;
			}
		}
	}
	else{
		if(forward>0){
			frames = &actual[3].frames;
		}
		else{
			frames = &actual[4].frames;
		}
	}
	
	
	if(right>0 && abs(forward)<0.92387953251){
		inverted = false;
	}
	else{
		inverted = true;
	}
}

void IdleAnim::onUpdate(UAnimation* anim)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Idle"));
	auto pawn = Cast<AEnemy>(anim->GetOwner());
	auto steering = pawn->steering;

	if(steering->velocity.Size()>1){
		anim->setWalkAnim();
	}
}


void WalkAnim::onUpdate(UAnimation* anim)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Walk"));
	auto pawn = Cast<AEnemy>(anim->GetOwner());
	auto steering = pawn->steering;

	if(steering->velocity.Size()<1){
		anim->setIdleAnim();
	}
}

void AnimState::onUpdate(UAnimation* anim)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("None"));
}
