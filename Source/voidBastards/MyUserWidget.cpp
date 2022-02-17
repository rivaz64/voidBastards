// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"

void UMyUserWidget::fire()
{
  GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("fire"));
  isFire = true;
  timer = .1f;
}

void UMyUserWidget::update(float deltaTime)
{
  if(isFire){
    timer-=deltaTime;
    isFire = timer>0;
  }
}
