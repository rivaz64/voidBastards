// Fill out your copyright notice in the Description page of Project Settings.


#include "Banger.h"

void
UBanger::shot(){
  fire();
}

void
UBanger::fire(){

  bulletSpawn(getDirection());

}