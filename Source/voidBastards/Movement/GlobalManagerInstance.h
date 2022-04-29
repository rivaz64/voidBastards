// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Serialization/StructuredArchive.h"

#include "GlobalManagerInstance.generated.h"

class FJsonValue;

class AEntity;

UENUM(BlueprintType)
enum class eACTION_TYPE : uint8
{
  kNone = 0 UMETA(DisplayName = "NONE"),
  kSetTimeVarFromVar UMETA(DisplayName = "SetTimeVarFromVar"),
  kSetTimeVarFromValue UMETA(DisplayName = "SetTimeVarFromValue"),
  kSetIntVarFromVar UMETA(DisplayName = "SetIntVarFromVar"),
  kSetIntVarFromValue UMETA(DisplayName = "SetIntVarFromValue"),
  kSetFloatVarFromVar UMETA(DisplayName = "SetFloatVarFromVar"),
  kSetFloatVarFromValue UMETA(DisplayName = "SetFloatVarFromValue"),
  kSetBoolVarFromVar UMETA(DisplayName = "SetBoolVarFromVar"),
  kSetBoolVarFromValue UMETA(DisplayName = "SetBoolVarFromValue"),
  kSetStringVarFromVar UMETA(DisplayName = "SetStringVarFromVar"),
  kSetStringVarFromValue UMETA(DisplayName = "SetStringVarFromValue"),
  kHealFromValue UMETA(DisplayName = "HealFromValue"),
  kHealFromVar UMETA(DisplayName = "HealFromVar"),
  kDealDamageFromValue UMETA(DisplayName = "DealDamageFromValue"),
  kDealDamageFromVar UMETA(DisplayName = "DealDamageFromVar"),
  kGiveMission UMETA(DisplayName = "Give mission"),
};

/**
 * 
 */
UCLASS()
class VOIDBASTARDS_API UGlobalManagerInstance : public UGameInstance
{
	GENERATED_BODY()

  UGlobalManagerInstance();

public:


  /////////////////////////////////////////////////////////////////////////////
  /*                                                                         */
  /*                          Collision Management                           */
  /*                                                                         */
  /////////////////////////////////////////////////////////////////////////////

  /* Functions */
  UFUNCTION(BlueprintCallable)
  bool GetCollisionsVertical(FVector Pos,
                             FVector& outDownPos,
                             float& outDownDistance,
                             FVector4& outDownPlane,
                             FVector& outUpPos,
                             float& outUpDistance,
                             FVector4& outUpPlane);



  /* Variables */
  TArray<class ATerrain*> GameTerrains;



 
};
