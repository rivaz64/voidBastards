// Fill out your copyright notice in the Description page of Project Settings.


#include "Regulator.h"

void
URegulator::fire(){
  bulletTrace(getDirection(),2000);
}