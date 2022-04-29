#pragma once
#include "Prerequisites.h"

namespace Harmony{

namespace PAWNS{
enum E{
  GOER=0,
  VIEW,
  CHANGER,
  WANDERER
};
}

/**
 * @brief Harmony itself
*/
class Manager
{
 public:

  Manager();

  ~Manager();

  /**
   * @brief adds a pawn to be controlled by harmony
   * @param  
  */
  void
  addPawn(Pawn* pawn, PAWNS::E type);

  /**
   * @brief updates all the controllers
   * @param delta 
  */
  void
  update(float delta);

 private:
  vector<Controller*> controllers;
  map<string,State*> states;

};

}


