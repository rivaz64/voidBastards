#pragma once
#include "Harmony/State.h"

namespace Harmony{

class AttackPlayer:
  public State
{
 public:

  virtual void
  update(Controller*,float delta) override;

  float aceptanceRadius = 100;
};

}

