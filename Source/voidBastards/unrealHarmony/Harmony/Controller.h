#pragma once

#include "Prerequisites.h"
#include "BlackBoard.h"
#include "Delegate.h"

namespace Harmony{

enum class MESSAGES
{
  OnEnter,
  OnExit,
  OnFinish,
  OnSeen
};

enum class STATES
{
  Wander,
  Pursue,
  Attack
};


struct DelegatorDesciption{
  uint fromState;
  uint message;
  Delegator* toState;
};

/**
 * @brief a state machine that controlls a pawn
*/
class Controller
{
 public:
  
  Controller(const map<uint,State*>& states);

  virtual 
  ~Controller();

  void
  init(vector<DelegatorDesciption> defaultReactions,
       vector<DelegatorDesciption> specificReactions);

  /**
   * @brief changes this state machine to a new state
   * @param newState the state to where is changing
  */
  void
  ChangeToState(uint newState);

  /**
   * @brief updates the state machine
  */
  void
  update(float delta);

  /**
   * @brief reacts to a recived message
   * @param msg 
  */
  void
  message(uint msg);

  /**
   * @brief chooses a new random location to go if the controller does not have a location to go
  */
  void
  newRandomPointToGo();

  /**
   * @brief goes to a point using the system the controller has
   * @param point 
  */
  virtual void
  goToPoint(const Dimencion& point,float radius);

  /**
   * @brief gets a random point that can be reached from a certain radius
   * @param point 
   * @param radius 
  */
  virtual Dimencion
  reachablePointInRadius(const Dimencion& point, float radius);

  /**
   * @brief to recive a mesage and do not react
  */
  void
  nothing(){}

  inline Pawn*
  getPawn(){
    return m_pawn;
  }

  inline void
  setPawn(Pawn* pawn){
    m_pawn = pawn;
  }
  
 public:
  /**
   * @brief the things this controller remembers
  */
  BlackBoard m_memory;

 protected:

  /**
   * @brief the state in which this machine is
  */
  State* m_actualState;

  /**
   * @brief all the states this machine can be in
  */
  map<uint,State*> m_states;

  /**
   * @brief all the ways the controller can feel the world
  */
  map<uint,Detector*> m_sences;

  /**
   * @brief this are the ways the actor is activly feeling the world
  */
  map<uint,Detector*> m_activeSences;

  /**
   * @brief the pawn that is being controlled
  */
  Pawn* m_pawn;
 public:
  float m_wanderDelta = 36;
  float m_wanderRadius = 18;

  friend class Manager;
};

}


