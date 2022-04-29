// Fill out your copyright notice in the Description page of Project Settings.


#include "HarmonyController.h"
#include "UnrealController.h"
#include "Harmony\GoToPoint.h"
#include "Harmony/LookTo.h"
#include "Harmony\Reaction.h"
#include "UnrealPawn.h"
#include "../Animation.h"
#include "../Enemy.h"
void 
AHarmonyController::OnPossess(APawn* InPawn){
  Super::OnPossess(InPawn);

  GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("possessed"));

  auto wander = new Harmony::GoToPoint();
  wander->aceptanceRadius = wanderAceptanceRadius;
  auto pursue = new Harmony::GoToPoint();
  pursue->aceptanceRadius = pursueRadius;

  auto anims = Cast<UAnimation>(InPawn->GetComponentByClass(UAnimation::StaticClass()));

  m_controller = new Harmony::UnrealController({
  {(Harmony::uint)Harmony::STATES::Wander, wander},
  {(Harmony::uint)Harmony::STATES::Pursue, pursue},
  {(Harmony::uint)Harmony::STATES::Attack, new Harmony::LookTo()}
  });

  //auto tra = new Harmony::Transition((Harmony::uint)Harmony::STATES::Pursue);

  m_controller->init({

  {0,
  (Harmony::uint)Harmony::MESSAGES::OnEnter,
  Harmony::Delegate<>::createPtr<Harmony::Reaction,Harmony::Controller,&Harmony::Controller::nothing>(m_controller)},
  

  {0,
  (Harmony::uint)Harmony::MESSAGES::OnExit,
  Harmony::Delegate<>::createPtr<Harmony::Reaction,Harmony::UnrealController,&Harmony::UnrealController::stop>(m_controller)},
  

  {0,
  (Harmony::uint)Harmony::MESSAGES::OnFinish,
  Harmony::Delegate<>::createPtr<Harmony::Reaction,Harmony::Controller,&Harmony::Controller::nothing>(m_controller)},
  
  {0,
  (Harmony::uint)Harmony::MESSAGES::OnSeen,
  Harmony::Delegate<>::createPtr<Harmony::Reaction,Harmony::UnrealController,&Harmony::UnrealController::goToPlayer>(m_controller)},
  
  },{

  
  
  {(Harmony::uint)Harmony::STATES::Wander,
  (Harmony::uint)Harmony::MESSAGES::OnEnter,
  Harmony::Delegate<>::createPtr<Harmony::Reaction,UAnimation,&UAnimation::setWalkAnim>(anims)},
  
  
  {(Harmony::uint)Harmony::STATES::Wander,
  (Harmony::uint)Harmony::MESSAGES::OnFinish,
  Harmony::Delegate<>::createPtr<Harmony::Reaction,Harmony::Controller,&Harmony::Controller::newRandomPointToGo>(m_controller)},
  
  {(Harmony::uint)Harmony::STATES::Wander,
  (Harmony::uint)Harmony::MESSAGES::OnSeen,
  new Harmony::Transition((Harmony::uint)Harmony::STATES::Pursue,m_controller)},
  
  {(Harmony::uint)Harmony::STATES::Pursue,
  (Harmony::uint)Harmony::MESSAGES::OnEnter,
  Harmony::Delegate<>::createPtr<Harmony::Reaction,UAnimation,&UAnimation::setAlertAnim>(anims)},
  
  {(Harmony::uint)Harmony::STATES::Pursue,
  (Harmony::uint)Harmony::MESSAGES::OnFinish,
  new Harmony::Transition((Harmony::uint)Harmony::STATES::Attack,m_controller)},
  
  {(Harmony::uint)Harmony::STATES::Attack,
  (Harmony::uint)Harmony::MESSAGES::OnEnter,
  Harmony::Delegate<>::createPtr<Harmony::Reaction,UAnimation,&UAnimation::setAttackAnim>(anims)},
  
  {(Harmony::uint)Harmony::STATES::Attack,
  (Harmony::uint)Harmony::MESSAGES::OnFinish,
  new Harmony::Transition((Harmony::uint)Harmony::STATES::Wander,m_controller)}
  
  });
  m_controller->m_controller = this;
  m_controller->m_wanderDelta = wanderDelta;
  m_controller->m_wanderRadius = wanderRadius;

  auto pawn = new Harmony::UnrealPawn();
  m_controller->setPawn(pawn);
  pawn->m_pawn = InPawn;
}

 void 
 AHarmonyController::Tick(float DeltaTime){
   m_controller->update(DeltaTime);
 }


