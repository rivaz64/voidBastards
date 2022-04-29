#include "Pawn.h"
#include "Vector2f.h"

#ifdef _DEBUG
#include "Debuger.h"
#endif

namespace Harmony{

Dimencion 
Pawn::getPosition()
{
   #ifdef _DEBUG
   print("get position not implemented");
   #endif
  return Dimencion();
}

Dimencion
Pawn::getVelocity()
{
  #ifdef _DEBUG
  print("get velocity not implemented");
  #endif
  return Dimencion();
}

float
Pawn::getMaxVelocity()
{
  #ifdef _DEBUG
  print("get max velocity not implemented");
  #endif
  return 0.0f;
}

Dimencion 
Pawn::getDirection()
{
  #ifdef _DEBUG
  print("get direction not implemented");
  #endif
  return Dimencion();
}

void 
Pawn::acelerate(const Dimencion&)
{
  #ifdef _DEBUG
  print("acelerate not implemented");
  #endif
}

void 
Pawn::rotate(float)
{
  #ifdef _DEBUG
  print("rotate not implemented");
  #endif
}

}

