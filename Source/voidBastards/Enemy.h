// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Materials/MaterialInterface.h"
#include "Enemy.generated.h"

class USteering;
class UAnimation;
class Udamagable;
class UVision;
class AttackType;
class DieType;
class AfterShoot;

UCLASS()
class VOIDBASTARDS_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
  void 
	setNormalMaterial();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
  void 
	setHittedMaterial();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
  void 
	setPoisedMaterial();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
  void 
	setSpriteFront();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
  void 
	setSpriteBack();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
  void 
	setSpriteSide();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
  void 
	setSpriteFrontSide();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
  void 
	setSpriteBackSide();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
  void 
	normalSprite();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
  void 
	invertSprite();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
  void 
	died();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void
	printSprite();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void
	onPoised();

	void 
	die();

	void
	hitted();

	void
	poisoned();

  UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void
	setSpriteRotation(float angle);

	UFUNCTION(BlueprintCallable)
	void
	onSeen();

	void 
	attack();

	virtual void
	attackUpdate() {}

	float timer = 0;

	bool isHitted = false;

	bool isDead = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UMaterialInterface* basicM;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UMaterialInterface* hittedM;

	//UPROPERTY(EditAnywhere,BlueprintReadWrite)
	//UObject* sprite;

	//UPROPERTY(EditAnywhere,BlueprintReadWrite)
	//UAnimation* anims;

	//UPROPERTY(EditAnywhere,BlueprintReadWrite)
	//USteering* steering;

	//UPROPERTY(EditAnywhere,BlueprintReadWrite)
	//Udamagable* damagable;

	//UPROPERTY(EditAnywhere,BlueprintReadWrite)
	//UVision* vision;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<AActor> bullet;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool destroyOnDie = true;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int attackType=0;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int dieType=0;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int reactType=0;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float distanceToFlee=1000;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float attackImpulse=1000;

	AttackType* attacker; 

	DieType* dier;

	AfterShoot* reactor;

	FVector2D front = FVector2D(1.0f,0.0f);
};

class AttackType{
	public:
	virtual void attack(AEnemy*){}
};

class AttackShoot : public AttackType{
	public:
	void attack(AEnemy*) override;
	int shoots = 0;
};

class AttackSuicide : public AttackType{
	public:
	void attack(AEnemy*) override;
};

class AttackEscape : public AttackType{
	public:
	void attack(AEnemy*) override;
};

class DieType{
	public:
	virtual void die(AEnemy*){}
	virtual void destroy(AEnemy*){}
};

class DieNormal : public DieType{
	public:
	void die(AEnemy*) override;
	void destroy(AEnemy*) override;
};

class DieExplode : public DieType{
	public:
  void die(AEnemy*) override;

	void destroy(AEnemy*) override;
};

class AfterShoot{
 public:
  virtual void react(AEnemy*){}
};

class Flee : public AfterShoot{
 public:
  virtual void react(AEnemy*);
};