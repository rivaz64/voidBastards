#include "UnrealController.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

namespace Harmony{

//void UnrealController::newRandomPointToGo()
//{
//  auto world = m_controller->GetWorld();
//  auto myLocation = m_controller->GetPawn()->GetActorLocation();
//  auto navSys = UNavigationSystemV1::GetCurrent(world);
//  auto wanderPoint = 
//  m_memory.addVariableOfType<FVector>("pointToGo");
//  m_memory.setVariableAs<FVector>("pointToGo",wanderPoint);
//}

void 
UnrealController::goToPoint(const Dimencion& point,float radius)
{
  auto world = m_controller->GetWorld();
  auto pawn = m_controller->GetPawn();
  auto pawnLocation = pawn->GetActorLocation();
  pawnLocation += (point-pawnLocation).GetSafeNormal()*100.f;
  FHitResult result;
  bool hitted = world->LineTraceSingleByChannel(result,pawnLocation,point,ECollisionChannel::ECC_Visibility);
  //DrawDebugLine(world, pawnLocation, result.Location, FColor::Emerald, true, -1, 0, 10);
  auto dif = (result.Location-point).Size();
  //if(dif>50)
  //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("using pathfinder"));
  //else
  //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Going directly"));

  m_controller->MoveToLocation(point,radius,true,dif>50 && hitted);
  
}

Dimencion 
UnrealController::reachablePointInRadius(const Dimencion& point, float radius)
{
  auto world = m_controller->GetWorld();
  auto navSys = UNavigationSystemV1::GetCurrent(world);
  return navSys->GetRandomReachablePointInRadius(world,point,radius);
}

void 
UnrealController::goToPlayer()
{
  auto world = m_controller->GetWorld();
  auto player = UGameplayStatics::GetPlayerCharacter(world,0);
  m_memory.addVariableOfType<FVector>("pointToGo");
  m_memory.setVariableAs<FVector>("pointToGo",player->GetActorLocation());
}

void
UnrealController::stop()
{
  //m_controller->MoveToActor(m_controller->GetOwner());
  m_controller->StopMovement();
}

}

