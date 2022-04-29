// Fill out your copyright notice in the Description page of Project Settings.


#include "Terrain.h"
#include "Serialization/StructuredArchive.h"

// Sets default values
ATerrain::ATerrain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

  Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
  RootComponent = Mesh;
}

#include <string>
// Called when the game starts or when spawned
void ATerrain::BeginPlay()
{
	Super::BeginPlay();


  bool HasData = true;

  if (!Mesh->GetStaticMesh()) HasData = false;
  if (HasData && !Mesh->GetStaticMesh()->GetRenderData()) HasData = false;

  if (HasData && Mesh->GetStaticMesh()->GetRenderData()->LODResources.Num() > 0)
  {
    FPositionVertexBuffer* VertexBuffer = &Mesh->GetStaticMesh()->GetRenderData()->LODResources[0].VertexBuffers.PositionVertexBuffer;
     
    if (VertexBuffer)
    {
      const int32 VertexCount = VertexBuffer->GetNumVertices();
      for (int32 Index = 0; Index < VertexCount; ++Index)
      {
        //This is in the Static Mesh Actor Class, so it is location and tranform of the SMActor
        const FVector WorldSpaceVertexLocation = GetActorLocation() + GetTransform().TransformVector(VertexBuffer->VertexPosition(Index));
        //add to output FVector array
        Vertices.Add(WorldSpaceVertexLocation);
        FString msg = std::to_string(Index).c_str();
        msg.Append(": ");
        msg.Append(WorldSpaceVertexLocation.ToString());
      }
    }

    TArray<TaskTrace::FId> IndexBuffer;
    Mesh->GetStaticMesh()->GetRenderData()->LODResources[0].IndexBuffer.GetCopy(IndexBuffer);
    if (IndexBuffer.Num() > 0)
    {
      const int32 IndexSize = IndexBuffer.Num();
      for (int32 Index = 0; Index < IndexSize; Index+=3)
      {
        FVector A = Vertices[IndexBuffer[Index + 0]];
        FVector B = Vertices[IndexBuffer[Index + 1]];
        FVector C = Vertices[IndexBuffer[Index + 2]];
        FVector2D minC;
        FVector2D maxC;
        FTerrainTriangle tt;

        tt.Vertices.Add(A);
        minC = FVector2D(A.X, A.Y);
        maxC = FVector2D(A.X, A.Y);

        tt.Vertices.Add(B);
        if (B.X < minC.X)
          minC.X = B.X;
        if (B.Y < minC.Y)
          minC.Y = B.Y;
        if (B.X > maxC.X)
          maxC.X = B.X;
        if (B.Y > maxC.Y)
          maxC.Y = B.Y;

        tt.Vertices.Add(C);
        if (C.X < minC.X)
          minC.X = C.X;
        if (C.Y < minC.Y)
          minC.Y = C.Y;
        if (C.X > maxC.X)
          maxC.X = C.X;
        if (C.Y > maxC.Y)
          maxC.Y = C.Y;

        FVector u = B - A;
        FVector v = C - A;

        FVector n = FVector::CrossProduct(u, v);

        tt.a = n.X;
        tt.b = n.Y;
        tt.c = n.Z;

        tt.d = FVector::DotProduct(A, n);

        tt.minCorner = minC;
        tt.maxCorner = maxC;

        Triangles.Add(tt);


        if (minC.X < minCorner.X)
          minCorner.X = minC.X;
        if (minC.Y < minCorner.Y)
          minCorner.Y = minC.Y;
        if (maxC.X > maxCorner.X)
          maxCorner.X = maxC.X;
        if (maxC.Y > maxCorner.Y)
          maxCorner.Y = maxC.Y;
      }
    }
  }
}

// Called every frame
void ATerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ATerrain::GetCollisionsVertical(FVector Pos,
                                     FVector& outDownPos,
                                     float& outDownDistance,
                                     FVector4& outDownPlane,
                                     FVector& outUpPos,
                                     float& outUpDistance,
                                     FVector4& outUpPlane)
{
  TArray<FTerrainTriangle> orderTriangles;

  for (auto& t : Triangles)
  {
    FVector2D min = t.minCorner;
    FVector2D max = t.maxCorner;
    if (Pos.X >= t.minCorner.X && Pos.X <= t.maxCorner.X
      && Pos.Y >= t.minCorner.Y && Pos.Y <= t.maxCorner.Y)
    {
      if (IsOnTriangleVertical(Pos, t))
        orderTriangles.Add(t);
    }
  }

  FVector DownMinPos;
  float DownMinDist = -99999.9f;
  FVector4 DownMinPlane;
  FVector UpMinPos;
  float UpMinDist = 99999.9f;
  FVector4 UpMinPlane;
  for (auto& t : orderTriangles)
  {
    FVector collPos = GetCollisionPosVertical(Pos, t);

    float distZ = collPos.Z - Pos.Z;

    if (distZ <= 0.0f)
    {
      if (distZ > DownMinDist)
      {
        DownMinPos = collPos;
        DownMinDist = distZ;
        DownMinPlane = FVector4(t.a, t.b, t.c, t.d);
      }
    }
    else
    {
      if (distZ < UpMinDist)
      {
        UpMinPos = collPos;
        UpMinDist = distZ;
        UpMinPlane = FVector4(t.a, t.b, t.c, t.d);
      }
    }
  }

  outDownPos = DownMinPos;
  outDownDistance = -DownMinDist;
  outDownPlane = DownMinPlane;
  outUpPos = UpMinPos;
  outUpDistance = UpMinDist;
  outUpPlane = UpMinPlane;

  return DownMinDist > -99999.0f || UpMinDist < 99999.0f;
}

bool ATerrain::IsOnTriangleVertical(FVector Pos, FTerrainTriangle t)
{
  FVector AB = t.Vertices[1] - t.Vertices[0];
  FVector BC = t.Vertices[2] - t.Vertices[1];
  FVector CA = t.Vertices[0] - t.Vertices[2];
  FVector AP = t.Vertices[0] - Pos;
  FVector BP = t.Vertices[1] - Pos;
  FVector CP = t.Vertices[2] - Pos;

  float a = AB.X * AP.Y - AB.Y * AP.X;
  float b = BC.X * BP.Y - BC.Y * BP.X;
  float c = CA.X * CP.Y - CA.Y * CP.X;

  if (a > 0.0f)
  {
    return b >= 0.0f && c >= 0.0f;
  }
  else if (a < 0.0f)
  {
    return b <= 0.0f && c <= 0.0f;
  }
  else
  {
    return b * c >= 0.0f;
  }

  return false;
}

FVector ATerrain::GetCollisionPosVertical(FVector Pos, FTerrainTriangle t)
{
  Pos.Z = (t.d - t.a * Pos.X - t.b * Pos.Y) / t.c;
  return Pos;
}

