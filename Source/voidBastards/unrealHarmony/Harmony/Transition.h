#pragma once

#include "Prerequisites.h"
#include "Delegate.h"
#include "Controller.h"

namespace Harmony{

/**
 * @brief a way to change states in a certain event
*/
class Transition:
  public Delegate<uint>
{
 public:

  /**
   * @brief 
   * @param controller the controller that is using this transicion
   * @param newState the state that is going to be in
  */
  Transition(uint newState, Controller* control) :
    Delegate<uint>(Delegate<uint>::create<Transition,Controller,&Controller::ChangeToState>(control)),
    m_newState(newState)
  {}

  /**
   * @brief changes to the new state
  */
  void
  execute() override;

 protected:
  
  Transition() = default;

  /**
   * @brief the new state to go to
  */
  uint m_newState;

  friend class Delegate<uint>;
};

}


