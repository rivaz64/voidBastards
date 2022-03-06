// Fill out your copyright notice in the Description page of Project Settings.


#include "Toaster.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "../MyUserWidget.h"

void
UToaster::shotting(){
  
  timer += delta; 

  if(timer>.5){
		timer = 0;	
	}
  else{
    return;
  }

  if(energy >= 3){
    energy = 0;
    bulletTrace(getDirection(),2000);
  }
  else{
    ++energy;
  }

  TArray< UUserWidget * > widgets;
  UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(),widgets,UMyUserWidget::StaticClass());
  if(widgets.Num()==0) return;
  auto widget = Cast<UMyUserWidget>(widgets[0]);
  widget->charge = energy;
  widget->isCharging = true;
}

void UToaster::stopShotting()
{
  energy = -1;

  TArray< UUserWidget * > widgets;
  UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(),widgets,UMyUserWidget::StaticClass());
  if(widgets.Num()==0) return;
  auto widget = Cast<UMyUserWidget>(widgets[0]);
  widget->isCharging = false;
}

void
UToaster::shot(){

}