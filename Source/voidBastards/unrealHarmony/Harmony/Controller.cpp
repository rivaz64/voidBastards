#include "Controller.h"
#include "State.h"
#include "Transition.h"
#include "Pawn.h"

namespace Harmony{

Controller::Controller(const map<uint,State*>& states) :
  m_states(states)                                  
{
  m_actualState = states.begin()->second;
}

Controller::~Controller()
{
  size_t numStates = m_states.size();
  for(size_t i=0;i<numStates;++i){
    delete m_states[i];
  }
}

void 
Controller::init(vector<DelegatorDesciption> defaultReactions, 
                 vector<DelegatorDesciption> specificReactions)
{
  map<uint,Delegator*> defaults;

  for(auto& desc : defaultReactions){
    defaults.insert({desc.message,desc.toState});
  }

  for(auto& state: m_states){
    state.second->m_reactions = defaults;
  }

  for(auto& desc : specificReactions){
    m_states[desc.fromState]->m_reactions[desc.message] = desc.toState;
  }
}

void
Controller::ChangeToState(uint newState)
{
  m_actualState->onMessage((uint)MESSAGES::OnExit);
  m_actualState = m_states[newState];
  m_actualState->onMessage((uint)MESSAGES::OnEnter);
}

void 
Controller::update(float delta)
{
  m_actualState->update(this,delta);
}

void 
Controller::message(uint msg)
{
  m_actualState->onMessage(msg);
}

void 
Controller::newRandomPointToGo()
{
  auto location = m_pawn->getPosition();
  auto direction = m_pawn->getDirection();
  auto newPoint = location+direction*m_wanderDelta;
  auto wanderPoint = reachablePointInRadius(newPoint,m_wanderRadius);
  m_memory.addVariableOfType<Dimencion>("pointToGo");
  m_memory.setVariableAs<Dimencion>("pointToGo",wanderPoint);
}

void
Controller::goToPoint(const Dimencion& point,float radius)
{
  print("goToPoint does nothing");
}

Dimencion 
Controller::reachablePointInRadius(const Dimencion& point, float radius)
{
  print("reachablePointInRadius nothing");
  return Dimencion();
}

}
