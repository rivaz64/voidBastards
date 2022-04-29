#include "Reaction.h"

namespace Harmony{

void 
Reaction::execute()
{
  (*this)();
}

}
