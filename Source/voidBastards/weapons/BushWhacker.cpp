// Fill out your copyright notice in the Description page of Project Settings.


#include "BushWhacker.h"

void
UBushWhacker::shot(){
  fire();
}

void
UBushWhacker::fire(){

  bulletSpawn(getDirection());

}