#pragma once

#include "Harmony/Controller.h"
#include "AIController.h"

namespace Harmony{

class UnrealController:
  public Controller
{
 public:

  UnrealController(const map<uint,State*>& states):
    Controller(states){}

  //void
  //newRandomPointToGo();

  void
  goToPoint(const Dimencion& point,float radius) override;

  Dimencion
  reachablePointInRadius(const Dimencion& point, float radius) override;

  void
  goToPlayer();

  void
  stop();

  AAIController* m_controller;
};

}


