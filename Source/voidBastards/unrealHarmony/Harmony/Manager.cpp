#include "Manager.h"
#include "Controller.h"
#include "Pawn.h"
#include "Controller.h"
#include "GoToPoint.h"
#include "LookTo.h"
#include "Reaction.h"
namespace Harmony{



Manager::Manager()
{
	states.insert({"GoToPoint",new GoToPoint});
	states.insert({"LookTo",new LookTo});
}

Manager::~Manager()
{
	size_t numControllers = controllers.size();
  for(size_t i=0;i<numControllers;++i){
    delete controllers[i];
  }
}

void
Manager::addPawn(Pawn* pawn, PAWNS::E type)
{
	Controller* newControl = nullptr;
	switch (type)
	{
	//case PAWNS::GOER:
	//	newControl = new Controller({{(uint)STATES::Wander,states["GoToPoint"]}}); 
	//	break;
	//
	//case PAWNS::VIEW:
	//	newControl = new Controller({{(uint)STATES::Wander,states["lookTo"]}}); 
	//	break;
	//
	//case PAWNS::CHANGER:
	//	newControl = new Controller({states["GoToPoint"],states["LookTo"]});
	//	newControl->init({
	//	{0,
	//	(uint)Messages::OnFinish,
	//	new Transition((uint)STATES::LookTo,newControl)}}
	//	,{}); 
	//	break;

	case PAWNS::WANDERER:

		newControl = new Controller({{(uint)STATES::Wander,states["GoToPoint"]}});
		newControl->init({
		{0,
		(uint)MESSAGES::OnFinish,
		Delegate<>::createPtr<Reaction,Controller,&Controller::newRandomPointToGo>(newControl)}}
		,{}); 

	default:
		break;
	}
	controllers.push_back(newControl);
  pawn->m_controller = newControl;
	newControl->m_pawn = pawn;
}

void 
Manager::update(float delta)
{
	for(auto& control : controllers){
		control->update(delta);
	}
}

}

