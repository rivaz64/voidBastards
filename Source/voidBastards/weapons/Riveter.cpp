// Fill out your copyright notice in the Description page of Project Settings.


#include "Riveter.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "../MyUserWidget.h"

void
URiveter::shotting(){

  timer += delta; 

  if(timer>.05){
		timer = 0;	

    TArray< UUserWidget * > widgets;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(),widgets,UMyUserWidget::StaticClass());
    if(widgets.Num()==0) return;
    auto widget = Cast<UMyUserWidget>(widgets[0]);

    if(flipFloop){
      flipFloop = false;

      widget->fire();
      //widget->fire = energy;
    }
    else{
      flipFloop = true;

      //widget->isFire = false;
    }
	}
  else{
    return;
  }

  auto front = getDirection();
  auto right = getDirectionRight();
  auto up = getDirectionUp();

  float x;
  float y;

  x=(FMath::FRand()*2.f-1.f)*separation;
  y=(FMath::FRand()*2.f-1.f)*separation;
  auto dir = front+right*x+up*y;

  bulletTrace(dir,2000);
}

void
URiveter::stopShotting()
{
  TArray< UUserWidget * > widgets;
  UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(),widgets,UMyUserWidget::StaticClass());
  if(widgets.Num()==0) return;
  auto widget = Cast<UMyUserWidget>(widgets[0]);

  //widget->isFire = false;
}

void
URiveter::shot(){

}