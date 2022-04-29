#pragma once
#include "Delegate.h"

namespace Harmony{

/**
 * @brief the way a controller will react
*/
class Reaction :
  public Delegate<>
{
 public:
  Reaction() = default;

  void
  execute() override;
};

}


