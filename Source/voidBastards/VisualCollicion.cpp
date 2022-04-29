// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualCollicion.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UVisualCollicion::UVisualCollicion()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	collidingVector = FVector::ZeroVector;
	// ...
}


// Called when the game starts
void UVisualCollicion::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UVisualCollicion::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto pawn = GetOwner();

	FHitResult result;
	auto feetPos = feets+pawn->GetActorLocation();
  GetWorld()->LineTraceSingleByChannel(result,feetPos,feetPos-FVector(0,0,collicionDistance),ECollisionChannel::ECC_Visibility);
	isInFloor = result.ImpactPoint!=FVector::ZeroVector; 

	if(debugFloor){
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%f"), FVector::Distance(pawn->GetActorLocation(),result.ImpactPoint)));
	}

	if(isInFloor && FVector::Distance(pawn->GetActorLocation(),result.ImpactPoint)<minFloorDist){
		pawn->SetActorLocation(result.ImpactPoint+FVector(0,0,minFloorDist));
	}	

	if(!isInFloor)
	upVel-=gravity;
	else if(upVel<0){
	upVel=0;
	}
	pawn->AddActorWorldOffset(FVector::UpVector*upVel,true);

	FVector colicionDir;
	isColliding = false;
	float angle = atan2f(pawn->GetActorForwardVector().Y,pawn->GetActorForwardVector().X);
	float anglePortion = PI/static_cast<float>(rounds);
	for(int round = 0; round<rounds;++round){
		angle+=anglePortion*static_cast<float>(round);
		colicionDir.X = cos(angle);
		colicionDir.Y = sin(angle);
		auto origin = pawn->GetActorLocation()+colicionDir*radialOrigin;
		GetWorld()->LineTraceSingleByChannel(result,origin,pawn->GetActorLocation()+colicionDir*radialColicion,ECollisionChannel::ECC_Visibility);
		isColliding = result.ImpactPoint!=FVector::ZeroVector; 
		if(isColliding){
			if(debugLine){
				DrawDebugLine(GetWorld(), pawn->GetActorLocation(), pawn->GetActorLocation()+colicionDir*radialColicion, FColor::Emerald, true, -1, 0, 10);
			}
			collidingVector = colicionDir;
			pawn->AddActorWorldOffset(-collidingVector*(radialColicion-FVector::Distance(pawn->GetActorLocation(),result.ImpactPoint)),true);
			break;
		}
		
	}
}

