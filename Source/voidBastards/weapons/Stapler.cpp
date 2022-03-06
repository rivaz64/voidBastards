// Fill out your copyright notice in the Description page of Project Settings.


#include "Stapler.h"

void
UStapler::fire(){
  
  auto front = getDirection();
  auto right = getDirectionRight();
  auto up = getDirectionUp();

  float x;
  float y;

  for(int i= 0;i<bulletsPerShot;++i){
    x=(FMath::FRand()*2.f-1.f)*separation;
    y=(FMath::FRand()*2.f-1.f)*separation;
    auto dir = front+right*x+up*y;
    bulletTrace(dir,750);
  }

}