// Fill out your copyright notice in the Description page of Project Settings.


#include "Clusterflack.h"

void
UClusterflack::shot(){
  fire();
}

void
UClusterflack::fire(){

  bulletSpawn(getDirection());

}