#pragma once

#include "Prerequisites.h"
#include "Vector2f.h"

namespace Harmony{

/**
 * @brief the interface class for what is going to be controlled
*/
class Pawn
{
 public:
  /**
   * @brief gets the position of this pawn
   * @return 
  */
  virtual Dimencion
  getPosition();

  /**
   * @brief gets the velocity of this pawn
   * @return 
  */
  virtual Dimencion
  getVelocity();

  /**
   * @brief gets the maximum velocity of this pawn can go
   * @return 
  */
  virtual float
  getMaxVelocity();

  /**
   * @brief gets the direction of this pawn
   * @return 
  */
  virtual Dimencion
  getDirection();

  /**
   * @brief acelerates the pawn in a certain direction;
   * @return 
  */
  virtual void
  acelerate(const Dimencion&);

  /**
   * @brief rotates the pawn in a certain direction;
   * @return 
  */
  virtual void
  rotate(float);

  inline Controller*
  getController(){
    return m_controller;
  }

  inline void
  setController(Controller* controller){
    m_controller = controller;
  }

 private:
  
  /**
   * @brief this is controlling this pawn
  */
  Controller* m_controller; 

  friend class Manager;
};

}


