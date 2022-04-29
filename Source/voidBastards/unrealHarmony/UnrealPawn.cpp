#include "UnrealPawn.h"
#include "Harmony/AngleUtilities.h"
#include "../Enemy.h"

namespace Harmony{

Dimencion 
UnrealPawn::getPosition()
{
  return m_pawn->GetActorLocation();
}

Dimencion
UnrealPawn::getVelocity()
{
  return m_pawn->GetVelocity();
}

Dimencion 
UnrealPawn::getDirection()
{
  return AngleToVector(m_pawn->GetActorRotation().Yaw*0.0174532924);
}

void
UnrealPawn::rotate(float v)
{
  //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%f"), v));
  //auto vec = Harmony::AngleToVector(v);
  //Cast<AEnemy>(m_pawn)->front = *reinterpret_cast<FVector2D*>(&vec);
  //m_pawn->SetPivotOffset(m_pawn->GetActorLocation());
  //m_pawn->SetLockLocation(true);
  m_pawn->SetActorRotation(FRotator(0,v*57.29578,0),ETeleportType::TeleportPhysics);
}

}

