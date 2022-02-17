// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "../MyUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "../Impact.h"
#include "../Enemy.h"

void 
UWeapon::use(){
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%d"),bullets));

  if(bullets > 0){
    shot();
    bullets--;
  }
  else{
    empty();

    return;
  }
  

}

FVector UWeapon::getLocation()
{
  return UGameplayStatics::GetPlayerCameraManager(world,0)->GetCameraLocation();
}

FVector UWeapon::getDirection()
{
  return UGameplayStatics::GetPlayerCameraManager(world,0)->GetActorForwardVector();
}

FVector UWeapon::getDirectionRight()
{
  return UGameplayStatics::GetPlayerCameraManager(world,0)->GetActorRightVector();
}

FVector UWeapon::getDirectionUp()
{
  return UGameplayStatics::GetPlayerCameraManager(world,0)->GetActorUpVector();
}