#pragma once
#include "State.h"

namespace Harmony{

/**
 * @brief the pawn stayis in place and looks to an objective
*/
class LookTo :
  public State
{
 public:
  void
  update(Controller* controller,float delta) override;

  float rotateRate;
};

}


