// Fill out your copyright notice in the Description page of Project Settings.


#include "Vision.h"
#include "Steering.h"
#include "Animation.h"
#include "damagable.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UVision::UVision()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UVision::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UVision::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	seeing = false;

	auto pawn = Cast<AActor>(GetOwner());

	//auto damagable = Cast<Udamagable>(pawn->GetComponentByClass(Udamagable::StaticClass()));


	auto location = FVector2D(pawn->GetActorLocation().X,pawn->GetActorLocation().Y);

	auto steering = Cast<USteering>(pawn->GetComponentByClass(USteering::StaticClass()));

	auto animation = Cast<UAnimation>(pawn->GetComponentByClass(UAnimation::StaticClass()));

	TArray<AActor*> found;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(),APawn::StaticClass(),found);

	for(auto other : found){
		if(other==pawn) continue;
		auto otherLocation = FVector2D(other->GetActorLocation().X,other->GetActorLocation().Y);
		auto dif = otherLocation-location;
		auto difSize = dif.Size();
		if(difSize<near || difSize<distance && FVector2D::DotProduct(dif.GetSafeNormal(),steering->forward)>angle){
			steering->pointToGo = otherLocation;
			steering->actualState->onSeen(steering);
			seeing = true;
		}
	}


}

