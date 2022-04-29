#include "State.h"
#include "Debuger.h"

namespace Harmony{

void 
State::onMessage(uint msg)
{
  #ifdef _DEBUG
  if(m_reactions.find(msg)==m_reactions.end()){
    print("unhandled message");
    print(msg);
    return;
  }
  #endif
  m_reactions[msg]->execute();
}

}

