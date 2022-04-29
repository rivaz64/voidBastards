// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementStateLink.h"

MovementStateLink::MovementStateLink(TArray<MovementState*> trueLink)
{
  TrueNodes = trueLink;
}

MovementStateLink::MovementStateLink(TArray<MovementState*> trueLink,
                                     TArray<MovementState*> falseLink,
                                     void* CondVal1,
                                     eVALUE_TYPE CondValType1,
                                     void* CondVal2,
                                     eVALUE_TYPE CondValType2,
                                     eOPERATOR_TYPE CondOp)
{
  HasCondition = true;
  TrueNodes = trueLink;
  FalseNodes = falseLink;
  ConditionValue1 = CondVal1;
  ConditionValueType1 = CondValType1;
  ConditionValue2 = CondVal2;
  ConditionValueType2 = CondValType2;
  ConditionOperator = CondOp;
}

bool MovementStateLink::Update(float DeltaTime, FVector& outMovement)
{
  if (HasCondition)
  {
    if (CheckConditions())
    {
      bool HasExecuted = false;
      for (auto tn : TrueNodes)
      {
        FVector mov = FVector(0.0f, 0.0f, 0.0f);
        if (tn->Update(DeltaTime, mov))
        {
          outMovement += mov;
          HasExecuted = true;
        }
      }
      return HasExecuted;
    }
    else
    {
      bool HasExecuted = false;
      for (auto tn : FalseNodes)
      {
        FVector mov = FVector(0.0f, 0.0f, 0.0f);
        if (tn->Update(DeltaTime, mov))
        {
          outMovement += mov;
          HasExecuted = true;
        }
      }
      return HasExecuted;
    }
  }
  else
  {
    bool HasExecuted = false;
    for (auto tn : TrueNodes)
    {
      FVector mov = FVector(0.0f, 0.0f, 0.0f);
      if (tn->Update(DeltaTime, mov))
      {
        outMovement += mov;
        HasExecuted = true;
      }
    }
    return HasExecuted;
  }

  return false;
}

bool MovementStateLink::CheckExit()
{
  if (HasCondition)
  {
    if (CheckConditions())
    {
      bool HasExecuted = false;
      for (auto tn : TrueNodes)
      {
        if (tn->CheckExit())
        {
          HasExecuted = true;
        }
      }
      for (auto tn : FalseNodes)
      {
        tn->TryExit();
      }
      return HasExecuted;
    }
    else
    {
      bool HasExecuted = false;
      for (auto tn : FalseNodes)
      {
        if (tn->CheckExit())
        {
          HasExecuted = true;
        }
      }
      for (auto tn : TrueNodes)
      {
        tn->TryExit();
      }
      return HasExecuted;
    }
  }
  else
  {
    bool HasExecuted = false;
    for (auto tn : TrueNodes)
    {
      if (tn->CheckExit())
      {
        HasExecuted = true;
      }
    }
    for (auto tn : FalseNodes)
    {
      tn->TryExit();
    }
    return HasExecuted;
  }

  return false;
}

void MovementStateLink::TryExit()
{
  for (auto tn : TrueNodes)
  {
    tn->TryExit();
  }
  for (auto tn : FalseNodes)
  {
    tn->TryExit();
  }
}

bool MovementStateLink::CheckConditions()
{
  bool MetCondition = false;
  switch (ConditionValueType1)
  {
  case kINT:
  {
    int* val1 = reinterpret_cast<int*>(ConditionValue1);
    switch (ConditionValueType2)
    {
    case kINT:
    {
      int* val2 = reinterpret_cast<int*>(ConditionValue2);
      switch (ConditionOperator)
      {
      case kLESS:
        MetCondition = *val1 < *val2;
        break;
      case kLESS_OR_EQUAL:
        MetCondition = *val1 <= *val2;
        break;
      case kGREATER:
        MetCondition = *val1 > *val2;
        break;
      case kGREATER_OR_EQUAL:
        MetCondition = *val1 >= *val2;
        break;
      case kEQUAL:
        MetCondition = *val1 == *val2;
        break;
      case kNOT_EQUAL:
        MetCondition = *val1 != *val2;
        break;
      }
    }
    break;
    case kFLOAT:
    {
      float* val2 = reinterpret_cast<float*>(ConditionValue2);
      switch (ConditionOperator)
      {
      case kLESS:
        MetCondition = static_cast<float>(*val1) < *val2;
        break;
      case kLESS_OR_EQUAL:
        MetCondition = static_cast<float>(*val1) <= *val2;
        break;
      case kGREATER:
        MetCondition = static_cast<float>(*val1) > *val2;
        break;
      case kGREATER_OR_EQUAL:
        MetCondition = static_cast<float>(*val1) >= *val2;
        break;
      case kEQUAL:
        MetCondition = static_cast<float>(*val1) == *val2;
        break;
      case kNOT_EQUAL:
        MetCondition = static_cast<float>(*val1) != *val2;
        break;
      }
    }
    break;
    }
  }
  break;
  case kFLOAT:
  {
    float* val1 = reinterpret_cast<float*>(ConditionValue1);
    switch (ConditionValueType2)
    {
    case kINT:
    {
      int* val2 = reinterpret_cast<int*>(ConditionValue2);
      switch (ConditionOperator)
      {
      case kLESS:
        MetCondition = *val1 < static_cast<float>(*val2);
        break;
      case kLESS_OR_EQUAL:
        MetCondition = *val1 <= static_cast<float>(*val2);
        break;
      case kGREATER:
        MetCondition = *val1 > static_cast<float>(*val2);
        break;
      case kGREATER_OR_EQUAL:
        MetCondition = *val1 >= static_cast<float>(*val2);
        break;
      case kEQUAL:
        MetCondition = *val1 == static_cast<float>(*val2);
        break;
      case kNOT_EQUAL:
        MetCondition = *val1 != static_cast<float>(*val2);
        break;
      }
    }
    break;
    case kFLOAT:
    {
      float* val2 = reinterpret_cast<float*>(ConditionValue2);
      switch (ConditionOperator)
      {
      case kLESS:
        MetCondition = *val1 < *val2;
        break;
      case kLESS_OR_EQUAL:
        MetCondition = *val1 <= *val2;
        break;
      case kGREATER:
        MetCondition = *val1 > *val2;
        break;
      case kGREATER_OR_EQUAL:
        MetCondition = *val1 >= *val2;
        break;
      case kEQUAL:
        MetCondition = *val1 == *val2;
        break;
      case kNOT_EQUAL:
        MetCondition = *val1 != *val2;
        break;
      }
    }
    break;
    }
  }
  break;
  case kBOOL:
  {
    bool* val1 = reinterpret_cast<bool*>(ConditionValue1);
    switch (ConditionValueType2)
    {
    case kBOOL:
    {
      bool* val2 = reinterpret_cast<bool*>(ConditionValue2);
      switch (ConditionOperator)
      {
      case kEQUAL:
        MetCondition = *val1 == *val2;
        break;
      case kNOT_EQUAL:
        MetCondition = *val1 != *val2;
        break;
      }
    }
    break;
    }
  }
  break;
  case kSTRING:
  {
    FString* val1 = reinterpret_cast<FString*>(ConditionValue1);
    switch (ConditionValueType2)
    {
    case kSTRING:
    {
      FString* val2 = reinterpret_cast<FString*>(ConditionValue2);
      switch (ConditionOperator)
      {
      case kEQUAL:
        MetCondition = *val1 == *val2;
        break;
      case kNOT_EQUAL:
        MetCondition = *val1 != *val2;
        break;
      }
    }
    break;
    }
  }
  break;
  case kVECTOR:
  {
    FVector* val1 = reinterpret_cast<FVector*>(ConditionValue1);
    switch (ConditionValueType2)
    {
    case kVECTOR:
    {
      FVector* val2 = reinterpret_cast<FVector*>(ConditionValue2);
      switch (ConditionOperator)
      {
      case kEQUAL:
        MetCondition = *val1 == *val2;
        break;
      case kNOT_EQUAL:
        MetCondition = *val1 != *val2;
        break;
      }
    }
    break;
    }
  }
  break;
  }
  return MetCondition;
}
