// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovementState.h"

enum eVALUE_TYPE : uint8
{
  kVALNONE,
	kINT,
	kFLOAT,
	kBOOL,
	kSTRING,
	kVECTOR,
	kVALCOUNT
};

enum eOPERATOR_TYPE : uint8
{
  kOPNONE,
  kLESS,
	kLESS_OR_EQUAL,
	kGREATER,
	kGREATER_OR_EQUAL,
	kEQUAL,
	kNOT_EQUAL,
  kOPCOUNT
};

/**
 * 
 */
class VOIDBASTARDS_API MovementStateLink : public MovementState
{
public:
  MovementStateLink(TArray<MovementState*> trueLink);
  MovementStateLink(TArray<MovementState*> trueLink,
                    TArray<MovementState*> falseLink,
                    void* CondVal1,
                    eVALUE_TYPE CondValType1,
                    void* CondVal2,
                    eVALUE_TYPE CondValType2,
                    eOPERATOR_TYPE CondOp);
  ~MovementStateLink() = default;

  bool Update(float DeltaTime, FVector& outMovement) override;
  bool CheckExit() override;
  void TryExit() override;

private:
  bool CheckConditions();


  void* ConditionValue1 = nullptr;
  eVALUE_TYPE ConditionValueType1 = eVALUE_TYPE::kVALNONE;
  void* ConditionValue2 = nullptr;
  eVALUE_TYPE ConditionValueType2 = eVALUE_TYPE::kVALNONE;

	eOPERATOR_TYPE ConditionOperator = eOPERATOR_TYPE::kOPNONE;

	bool HasCondition = false;

  TArray<MovementState*> TrueNodes;
  TArray<MovementState*> FalseNodes;
};
