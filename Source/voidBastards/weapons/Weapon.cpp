// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "../MyUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "../Impact.h"
#include "../Enemy.h"
#include "Throwable.h"
#include "../damagable.h"


void 
UWeapon::use(){
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%d"),bullets));

  shot();
}

FVector UWeapon::getLocation()
{
  return UGameplayStatics::GetPlayerCameraManager(GetWorld(),0)->GetCameraLocation();
}

FVector UWeapon::getDirection()
{
  return UGameplayStatics::GetPlayerCameraManager(GetWorld(),0)->GetActorForwardVector();
}

FVector UWeapon::getDirectionRight()
{
  return UGameplayStatics::GetPlayerCameraManager(GetWorld(),0)->GetActorRightVector();
}

FVector UWeapon::getDirectionUp()
{
  return UGameplayStatics::GetPlayerCameraManager(GetWorld(),0)->GetActorUpVector();
}

void 
UWeapon::shot()
{
  TArray< UUserWidget * > widgets;
  UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(),widgets,UMyUserWidget::StaticClass());
  if(widgets.Num()==0) return;
  auto widget = Cast<UMyUserWidget>(widgets[0]);
  widget->fire();
    
  fire();
}

void
UWeapon::empty()
{
  TArray< UUserWidget * > widgets;
  UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(),widgets,UMyUserWidget::StaticClass());
  if(widgets.Num()==0) return;
  auto widget = Cast<UMyUserWidget>(widgets[0]);
  widget->isEmpty = true;
}

bool
UWeapon::damageEnemy(const FHitResult& result){
  auto impact = result.ImpactPoint;
  
  auto hittedActor = result.GetActor();
  auto enemy = Cast<AEnemy>(hittedActor);
 
  if(enemy){
    auto atitude =impact - hittedActor->GetActorLocation();
    float multiplier=1;
    if(atitude.Z>70){
      multiplier=2;
    }
    if(bulletDamages.Num()>0)
    Cast<Udamagable>(enemy->GetComponentByClass(Udamagable::StaticClass()))->hitted(bulletDamages[level]* multiplier);
    return true;
  }
  return false;
}

void
UWeapon::bulletTrace(FVector dir, float distance)
{
  //if(bullets > 0){
  //  //shot();
  //  //bullets--;
  //}
  //else{
  //  empty();
  //
  //  return;
  //}
  auto start = getLocation();
  auto end = start + dir*distance;
  FHitResult result;
  GetWorld()->LineTraceSingleByChannel(result,start,end,ECollisionChannel::ECC_Visibility);

  if(!result.GetActor()) return;

  if(damageEnemy(result)) return;

  auto impact = result.ImpactPoint;

  FVector NormalVector = result.ImpactNormal;
  
  FVector RotationAxis = FVector::CrossProduct(FVector::UpVector, NormalVector);
  RotationAxis.Normalize();
  
  float DotProduct = FVector::DotProduct(FVector::UpVector, NormalVector);
  float RotationAngle = acosf(DotProduct);
  
  FQuat Quat = FQuat(RotationAxis, RotationAngle);
  
  FTransform trans;
  trans.SetLocation(impact+ result.ImpactNormal);
  trans.SetRotation(Quat);
  if(marks.Num()>0)
  GetWorld()->SpawnActor<AActor>(marks[FMath::Rand()%marks.Num()],trans);
}

void 
UWeapon::bulletSpawn(FVector dir)
{
  FTransform trans;
  trans.SetLocation(getLocation()+dir*250);
  float angle = atan2f(getDirection().Y,getDirection().X);
  trans.SetRotation(FRotator(0,angle*57.29578f+90.f,0).Quaternion());
  FActorSpawnParameters SpawnInfo;
  SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
  SpawnInfo.bNoFail = true;
  auto object = GetWorld()->SpawnActor<AThrowable>(throwable,trans,SpawnInfo);

  object->mesh->AddImpulse(dir*impulse);
}
