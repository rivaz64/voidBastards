#pragma once
#include "GameFramework/Character.h"
#include "Harmony/Pawn.h"

namespace Harmony{

class UnrealPawn:
  public Pawn
{
 public:

  Dimencion
  getPosition() override; 

  Dimencion
  getVelocity() override;

  Dimencion
  getDirection() override;

  void
  rotate(float) override;


  APawn* m_pawn;
};

}


