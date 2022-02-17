// Fill out your copyright notice in the Description page of Project Settings.


#include "FireArm.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "../MyUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "../Impact.h"
#include "../Enemy.h"
#include "../damagable.h"


void
UFireArm::shot(){
  
  TArray< UUserWidget * > widgets;
  UWidgetBlueprintLibrary::GetAllWidgetsOfClass(world,widgets,UMyUserWidget::StaticClass());
  if(widgets.Num()==0) return;
  auto widget = Cast<UMyUserWidget>(widgets[0]);
  widget->fire();
    
  fire();
}

void 
UFireArm::bulletTrace(FVector dir, float distance)
{
  auto start = getLocation();
  auto end = start + dir*distance;
  FHitResult result;
  world->LineTraceSingleByChannel(result,start,end,ECollisionChannel::ECC_Visibility);

  if(!result.GetActor()) return;

  auto impact = result.ImpactPoint;
  
  auto hittedActor = result.GetActor();
  auto enemy = Cast<AEnemy>(hittedActor);
 
  if(enemy){
    auto atitude =impact - hittedActor->GetActorLocation();
    float multiplier=1;
    if(atitude.Z>70){
      multiplier=2;
    }
    enemy->damagable->hitted(bulletDamages[level]* multiplier);
    return;
  }

  FVector NormalVector = result.ImpactNormal;
  
  FVector RotationAxis = FVector::CrossProduct(FVector::UpVector, NormalVector);
  RotationAxis.Normalize();
  
  float DotProduct = FVector::DotProduct(FVector::UpVector, NormalVector);
  float RotationAngle = acosf(DotProduct);
  
  FQuat Quat = FQuat(RotationAxis, RotationAngle);
  
  FTransform trans;
  trans.SetLocation(impact+ result.ImpactNormal);
  trans.SetRotation(Quat);
  world->SpawnActor<AActor>(marks[FMath::Rand()%marks.Num()],trans);
}

void
UFireArm::empty(){
  TArray< UUserWidget * > widgets;
  UWidgetBlueprintLibrary::GetAllWidgetsOfClass(world,widgets,UMyUserWidget::StaticClass());
  if(widgets.Num()==0) return;
  auto widget = Cast<UMyUserWidget>(widgets[0]);
  widget->isEmpty = true;
}