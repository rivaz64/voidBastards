#include "Transition.h"
#include "Controller.h"
namespace Harmony{

void
Transition::execute()
{
  (*this)(m_newState);
}

}

