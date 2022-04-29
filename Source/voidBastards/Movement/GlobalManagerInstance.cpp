// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalManagerInstance.h"
#include "Kismet/GameplayStatics.h"

#include "Misc/FileHelper.h"
#include "Json.h"

#include "Terrain.h"
//#include "Game/Entity.h"
//#include "Game/Components/Effects_C.h"
//#include "Game/Components/MissionsBook_C.h"
//
//#include "Game/Props/LootBag.h"

#include "Runtime/Engine/Classes/Engine/UserInterfaceSettings.h"


UGlobalManagerInstance::UGlobalManagerInstance()
{
  //if (ItemsDescs.Num() == 0)
  //{
  //  LoadItemsDescriptions(TEXT("C:/Users/Mara May/Documents/GitHub/MMMORPG/Content/JSON/Items.json"));
  //}
  //
  //if (MissionsDescs.Num() == 0)
  //{
  //  LoadMissionDescriptions(TEXT("C:/Users/Mara May/Documents/GitHub/MMMORPG/Content/JSON/Missions.json"));
  //}
}


/////////////////////////////////////////////////////////////////////////////
/*                                                                         */
/*                          Collision Management                           */
/*                                                                         */
/////////////////////////////////////////////////////////////////////////////

bool
UGlobalManagerInstance::GetCollisionsVertical(FVector Pos,
                                              FVector& outDownPos,
                                              float& outDownDistance,
                                              FVector4& outDownPlane,
                                              FVector& outUpPos,
                                              float& outUpDistance,
                                              FVector4& outUpPlane)
{
  if (GameTerrains.Num() == 0)
  {
    TArray<AActor*> arrays;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATerrain::StaticClass(), arrays);

    for (auto a : arrays)
    {
      ATerrain* t = Cast<ATerrain>(a);
      if (t)
      {
        GameTerrains.Add(t);
      }
    }
  }

  TArray<ATerrain*> terrains;

  for (auto t : GameTerrains)
  {
    if (Pos.X >= t->minCorner.X && Pos.X <= t->maxCorner.X
      && Pos.Y >= t->minCorner.Y && Pos.Y <= t->maxCorner.Y)
      terrains.Add(t);
  }

  outDownDistance = 99999.9f;
  outUpDistance = 99999.9f;
  for (auto& t : terrains)
  {
    FVector DownMinPos;
    float DownMinDist;
    FVector4 DownMinPlane;
    FVector UpMinPos;
    float UpMinDist;
    FVector4 UpMinPlane;
    if (t->GetCollisionsVertical
       (
         Pos,
         DownMinPos,
         DownMinDist,
         DownMinPlane,
         UpMinPos,
         UpMinDist,
         UpMinPlane
        ))
    {
      if (DownMinDist < outDownDistance)
      {
        outDownPos = DownMinPos;
        outDownDistance = DownMinDist;
        outDownPlane = DownMinPlane;
      }
      if (UpMinDist < outUpDistance)
      {
        outUpPos = UpMinPos;
        outUpDistance = UpMinDist;
        outUpPlane = UpMinPlane;
      }
    }
  }

	return outDownDistance > -99999.0f || outUpDistance < 99999.0f;
}
