#include "GoToPlayer.h"
#include "UnrealController.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
namespace Harmony{

void
GoToPlayer::update(Controller* controller, float delta)
{
  auto& unrealControl = reinterpret_cast<UnrealController*>(controller)->m_controller;
  auto player = UGameplayStatics::GetPlayerCharacter(unrealControl->GetWorld(),0);
  auto pointToGo = controller->m_memory.getVariableAs<FVector>("pointToGo");
  auto myLocation = unrealControl->GetPawn()->GetActorLocation();

  if(!pointToGo){
    controller->message(static_cast<uint>(MESSAGES::OnFinish));
    return;
  }

  auto distance = *pointToGo-myLocation;

  if(distance.Size()<aceptanceRadius){
    controller->message(static_cast<uint>(MESSAGES::OnFinish));
  }

  unrealControl->MoveToLocation(*pointToGo);
}

}

