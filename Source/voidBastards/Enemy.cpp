// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Steering.h"
#include "Animation.h"
#include "damagable.h"
#include "Vision.h"
#include "weapons/Throwable.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "voidBastardsCharacter.h"
#include "unrealHarmony/HarmonyController.h"
#include "unrealHarmony/UnrealController.h"
#include "unrealHarmony/Harmony/AngleUtilities.h"
// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//anims = CreateDefaultSubobject<UAnimation>(TEXT("anims"));
	//steering = CreateDefaultSubobject<USteering>(TEXT("steering"));
	//damagable = CreateDefaultSubobject<Udamagable>(TEXT("damagable"));
	//vision = CreateDefaultSubobject<UVision>(TEXT("vision"));
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	if(attackType==0){
		attacker = new AttackShoot;
	}
	else{
		attacker = new AttackSuicide;
	}

	if(dieType==0){
		dier = new DieNormal;
	}
	else{
		dier = new DieExplode;
	}

	if(reactType==0){
		reactor = new AfterShoot;
	}
	else{
		reactor = new Flee;
	}

	//vida = 6;
	//forward = FVector2D(1,0);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	//sprite = anims->sprite;
	TArray<AActor*> found;
	 UGameplayStatics::GetAllActorsOfClass(GetWorld(),AvoidBastardsCharacter::StaticClass(),found);
	if(found.Num()==0) return;
	auto player =found[0];
	auto relaPos3d = (player->GetActorLocation()-GetActorLocation());
	auto relaPos = FVector2D(relaPos3d.X,relaPos3d.Y).GetSafeNormal();
	auto vel3d = GetVelocity();
	auto vel = FVector2D(vel3d.X,vel3d.Y);

	if(vel.Size()>.1){
		front = vel.GetSafeNormal();
		SetActorRotation(FRotator(0,atan2(front.Y,front.X)*57.29578,0));
	}
	else{
		auto angle = GetActorRotation().Yaw*0.0174532924;
		front = FVector2D(cos(angle),sin(angle));
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("x: %f, y: %f"), front.X, front.Y));


	auto pawn = Cast<AActor>(GetOwner());

	//auto steering = Cast<USteering>(pawn->GetComponentByClass(USteering::StaticClass()));

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("x: %f, y: %f,x: %f"), relaPos.X, relaPos.Y,relaPos.Z));

	float angle = FVector2D::DotProduct(relaPos,FVector2D(1,0));

	auto relaPos2 = FVector2D(relaPos.X,relaPos.Y);

	/*if(FVector2D::DotProduct(FVector2D(relaPos.X,relaPos.Y),steering->right)>0){
		anims->inverted = false;
	}
	else{
		anims->inverted = true;
	}*/

	auto anims = Cast<UAnimation>(GetComponentByClass(UAnimation::StaticClass()));
	if(anims)
	anims->setDirection(relaPos2);

	if(relaPos.Y>0) angle = acos(angle)*57.2957795131-90;
	else angle = asin( angle)*57.2957795131+180;

	setSpriteRotation(angle);

	Super::Tick(DeltaTime);
	if(isDead){
		timer-=DeltaTime;
		if(anims)
		anims->setDeadAnim();
		if(timer<0){
		if(destroyOnDie)
			dier->destroy(this);
			this->Destroy();
		}
		
	}
	else if(isHitted){
		timer-=DeltaTime;
		if(timer<0){
			isHitted=false;
			if(anims)
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
	//died();
	dier->die(this);
	//auto steering = GetComponentByClass(USteering::StaticClass());
	//if(steering)
	//Cast<USteering>(steering)->maxVelocity = 0;
	////steering->DestroyComponent();
	//auto vision = GetComponentByClass(UVision::StaticClass());
	//if(vision)
	//vision->DestroyComponent();
	//isDead = true;
	//setNormalMaterial();
	//auto anims = Cast<UAnimation>(GetComponentByClass(UAnimation::StaticClass()));
	//if(anims)
	//anims->setDeadAnim();
	////actualGroup = &deadAnim;
	//timer = 1;
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

void 
AEnemy::onSeen()
{
	auto control = GetController();
	if(control)
	Cast<AHarmonyController>(control)->m_controller->message((Harmony::uint)Harmony::MESSAGES::OnSeen);
}

void AEnemy::attack()
{
	attacker->attack(this);
}

void AttackShoot::attack(AEnemy* enemy)
{
	FTransform trans;
	//auto steering = Cast<USteering>(enemy->GetComponentByClass(USteering::StaticClass()));

	auto dir = FVector(enemy->front.X,enemy->front.Y,0);
	trans.SetLocation(enemy->GetActorLocation()+dir*100);
	auto projectil = enemy->GetWorld()->SpawnActor<AActor>(enemy->bullet,trans);
	//auto bullet = ;
	auto trowed = Cast<AThrowable>(projectil);
	trowed->mesh->AddImpulse(dir*enemy->attackImpulse);
	trowed->ThrowedBy = enemy;
	enemy->reactor->react(enemy);
	++shoots;
	if(shoots>=3){
		Cast<AHarmonyController>(enemy->GetController())->m_controller->message((int)Harmony::MESSAGES::OnFinish);
	}
}

void AttackSuicide::attack(AEnemy* enemy)
{
	
	enemy->die();
}

void AttackEscape::attack(AEnemy* enemy)
{
	FTransform trans;
	auto steering = Cast<USteering>(enemy->GetComponentByClass(USteering::StaticClass()));

	auto dir = FVector(steering->forward.X,steering->forward.Y,0);
	trans.SetLocation(enemy->GetActorLocation()+dir*100);
	auto projectil = enemy->GetWorld()->SpawnActor<AActor>(enemy->bullet,trans);
	//auto bullet = ;
	Cast<AThrowable>(projectil)->mesh->AddImpulse(dir*1000);

	//steering->actualState = steering 
}

void DieNormal::die(AEnemy* enemy)
{
	auto steering = enemy->GetComponentByClass(USteering::StaticClass());
	if(steering)
	Cast<USteering>(steering)->maxVelocity = 0;
	//steering->DestroyComponent();
	auto vision = enemy->GetComponentByClass(UVision::StaticClass());
	if(vision)
	vision->DestroyComponent();
	enemy->isDead = true;
	enemy->setNormalMaterial();
	auto anims = Cast<UAnimation>(enemy->GetComponentByClass(UAnimation::StaticClass()));
	if(anims)
	anims->setDeadAnim();
	//actualGroup = &deadAnim;
	enemy->timer = 1;
}

void DieNormal::destroy(AEnemy*)
{
}

void DieExplode::die(AEnemy* enemy)
{
	enemy->isDead = true;
	enemy->timer = 1;
}

void DieExplode::destroy(AEnemy* enemy)
{
	FTransform trans;
	trans.SetLocation(enemy->GetActorLocation());
	enemy->GetWorld()->SpawnActor<AActor>(enemy->bullet,trans);
}

void Flee::react(AEnemy* enemy)
{
	auto steering = Cast<USteering>(enemy->GetComponentByClass(USteering::StaticClass()));

	auto anim = Cast<UAnimation>(enemy->GetComponentByClass(UAnimation::StaticClass()));

	if(anim){
		anim->setEnrageAnim();
	}

	if(!steering) return;

	auto dir =  (FVector2D(enemy->GetActorLocation().X,enemy->GetActorLocation().Y)-steering->pointToGo).GetSafeNormal();

	DrawDebugLine(
	  enemy->GetWorld(),
	  enemy->GetActorLocation(),
	  FVector( dir*enemy->distanceToFlee,enemy->GetActorLocation().Z),
	  FColor(255, 0, 0),
	  false, 10, 0,
	  5.0f
	);
	

	steering->pointToGo = dir*enemy->distanceToFlee;

	steering->actualState = steering->fleeState;

	steering->velocity = dir;


}
