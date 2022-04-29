// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementState.h"
#include "MovementStateLink.h"

bool MovementState::Update(float DeltaTime, FVector& outMovement)
{
  bool CanExcecute = true;
  for (auto l : Links)
  {
    FVector mov = FVector(0.0f, 0.0f, 0.0f);
    if (l->Update(DeltaTime, mov))
    {
      outMovement += mov;
      CanExcecute = false;
    }
  }

  if (CanExcecute)
  {
    if (FirstUpdate)
    {
      Init();
      FirstUpdate = false;
    }

    return false;
  }
  return true;
}

bool MovementState::CheckExit()
{
  bool HasExecuted = false;
  for (auto l : Links)
  {
    if (l->CheckExit())
    {
      HasExecuted = true;
    }
  }

  if (HasExecuted && !FirstUpdate)
  {
    Exit();
    FirstUpdate = true;
  }
  return true;
}

void MovementState::TryExit()
{
  for (auto l : Links)
  {
    l->TryExit();
  }

  if (!FirstUpdate)
  {
    Exit();
    FirstUpdate = true;
  }
}

void MovementState::AddLink(MovementStateLink* newLink)
{
  Links.Add(newLink);
}
