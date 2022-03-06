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

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UAnimation* anims;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USteering* steering;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	Udamagable* damagable;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UVision* vision;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<AActor> bullet;

};
