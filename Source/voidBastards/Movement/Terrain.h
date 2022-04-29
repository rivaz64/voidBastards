// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Terrain.generated.h"

USTRUCT(BlueprintType)
struct FTerrainTriangle
{
  GENERATED_BODY()

  UPROPERTY()
  TArray<FVector> Vertices;

  float a = 0.0f;
  float b = 0.0f;
  float c = 0.0f;
  float d = 0.0f;

  FVector2D minCorner;
  FVector2D maxCorner;
};

UCLASS()
class VOIDBASTARDS_API ATerrain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATerrain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



  //////////////////////////////////////////////////////////////////////////////
  /*                                                                          */
  /*                                Components                                */
  /*                                                                          */
  //////////////////////////////////////////////////////////////////////////////

  UPROPERTY(EditAnywhere, Category = TerrainData)
  class UStaticMeshComponent* Mesh;


  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TerrainData)
  TArray<FVector> Vertices;
  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TerrainData)
  TArray<FTerrainTriangle> Triangles;

  FVector2D minCorner = FVector2D(99999.9f, 99999.9f);
  FVector2D maxCorner = FVector2D(-99999.9f, -99999.9f);;


  //////////////////////////////////////////////////////////////////////////////
  /*                                                                          */
  /*                                Functions                                 */
  /*                                                                          */
  //////////////////////////////////////////////////////////////////////////////

  UFUNCTION(BlueprintCallable)
  bool GetCollisionsVertical(FVector Pos,
                             FVector& outDownPos,
                             float& outDownDistance,
                             FVector4& outDownPlane,
                             FVector& outUpPos,
                             float& outUpDistance,
                             FVector4& outUpPlane);

  bool IsOnTriangleVertical(FVector Pos, FTerrainTriangle t);

  FVector GetCollisionPosVertical(FVector Pos, FTerrainTriangle t);
};
