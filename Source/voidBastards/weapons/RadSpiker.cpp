// Fill out your copyright notice in the Description page of Project Settings.


#include "RadSpiker.h"

void
URadSpiker::fire(){
  bulletSpawn(getDirection());
}