#include "GoToPoint.h"
#include "Pawn.h"
#include "Vector2f.h"
#include "AngleUtilities.h"
namespace Harmony{

void 
GoToPoint::update(Controller* controller,float delta)
{
  auto pawn = controller->getPawn();
  auto position = pawn->getPosition();
  auto pointToGo = controller->m_memory.getVariableAs<Dimencion>("pointToGo");

  if(!pointToGo){
    controller->message(static_cast<uint>(MESSAGES::OnFinish));
    return;
  }

  auto distance = *pointToGo-position;

  if(size(distance)<aceptanceRadius){
    controller->message(static_cast<uint>(MESSAGES::OnFinish));
  }

  controller->goToPoint(*pointToGo,aceptanceRadius-50);
}

}

