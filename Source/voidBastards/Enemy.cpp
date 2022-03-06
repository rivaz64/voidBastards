// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Steering.h"
#include "Animation.h"
#include "damagable.h"
#include "Vision.h"
#include "weapons/Throwable.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	anims = CreateDefaultSubobject<UAnimation>(TEXT("anims"));
	steering = CreateDefaultSubobject<USteering>(TEXT("steering"));
	damagable = CreateDefaultSubobject<Udamagable>(TEXT("damagable"));
	vision = CreateDefaultSubobject<UVision>(TEXT("vision"));
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	//vida = 6;
	//forward = FVector2D(1,0);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	//sprite = anims->sprite;

	
	auto player = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);

	auto relaPos = (player->GetActorLocation()-GetActorLocation()).GetSafeNormal();

	auto pawn = Cast<AActor>(GetOwner());

	//auto steering = Cast<USteering>(pawn->GetComponentByClass(USteering::StaticClass()));

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("x: %f, y: %f,x: %f"), relaPos.X, relaPos.Y,relaPos.Z));

	float angle = FVector::DotProduct(relaPos,GetActorForwardVector());

	auto relaPos2 = FVector2D(relaPos.X,relaPos.Y);

	/*if(FVector2D::DotProduct(FVector2D(relaPos.X,relaPos.Y),steering->right)>0){
		anims->inverted = false;
	}
	else{
		anims->inverted = true;
	}*/

	

	anims->setDirection(relaPos2);

	if(relaPos.Y>0) angle = acos(angle)*57.2957795131-90;
	else angle = asin( angle)*57.2957795131+180;

	setSpriteRotation(angle);

	Super::Tick(DeltaTime);
	if(isDead){
		timer-=DeltaTime;
		anims->setDeadAnim();
		if(timer<0){
			this->Destroy();
		}
		
	}
	else if(isHitted){
		timer-=DeltaTime;
		if(timer<0){
			isHitted=false;
			setNormalMaterial();
		}
	}
	/*
	if(!isDead){
		if(steering->velocity.Size()>10){
			anims->setWalkAnim();
		}
		else{
			anims->setIdleAnim();
		}
	}
	else{
		anims->setDeadAnim();
	}
	*/
	//if(inverted)
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

void AEnemy::die()
{
	died();
	steering->DestroyComponent();
	vision->DestroyComponent();
	isDead = true;
	setNormalMaterial();
	anims->setDeadAnim();
	//actualGroup = &deadAnim;
	timer = 1;
}

void AEnemy::hitted()
{
	setHittedMaterial();
	timer = .1;
	isHitted = true;
}

void AEnemy::poisoned()
{
	setPoisedMaterial();
	timer = .1;
	isHitted = true;
	
}

void AEnemy::attack()
{
	FTransform trans;
	auto dir = FVector(steering->forward.X,steering->forward.Y,0);
	trans.SetLocation(GetActorLocation()+dir*100);
	auto projectil = GetWorld()->SpawnActor<AActor>(bullet,trans);
	//Cast<AThrowable>(projectil)->mesh->AddImpulse(dir*1000);
}






