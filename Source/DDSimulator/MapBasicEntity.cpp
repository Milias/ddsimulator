// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
#include "MapState.h"
#include "MapBasicEntity.h"

AMapBasicEntity::AMapBasicEntity() : uid(0), MovingTime(0), MaxMovingTime(1.0f)
{
  PrimaryActorTick.bCanEverTick = true;
  bReplicates = true;
  bAlwaysRelevant = true;
  bFindCameraComponentWhenViewTarget = true;
  SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComponent"));
  Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
  Mesh->AttachTo(RootComponent);
  Mesh->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Meshes/Basic/Sphere.Sphere'")).Object);
}

void AMapBasicEntity::BeginPlay()
{
	Super::BeginPlay();
}

void AMapBasicEntity::Tick( float DeltaTime )
{
  Super::Tick(DeltaTime);
  if (MovingTiles.Num() > 1) {
    MovingTime += DeltaTime;
    if (MovingTime > MaxMovingTime) {
      MovingTime = 0;
      MovingTiles.RemoveAt(0);
    }
    if (MovingTiles.Num() == 1) {
      MovingTiles.RemoveAt(0);
      return;
    }
    SetActorLocation(FMath::Lerp<FVector, float>(MovingTiles[0]->EntityPosition(), MovingTiles[1]->EntityPosition(), MovingTime/MaxMovingTime));
  }
}

void AMapBasicEntity::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
}

void AMapBasicEntity::AssignTiles_Implementation(const TArray<UMapTile*>& tiles)
{
  for (auto it = tiles.CreateConstIterator(); it; ++it) {
    (*it)->AssignedEntity.Add(this);
  }
}

bool AMapBasicEntity::AssignTiles_Validate(const TArray<UMapTile*>& tiles)
{
  return true;
}

void AMapBasicEntity::MoveTo_Implementation(const TArray<int32>& tiles)
{
  TArray<int32> t; t.SetNumZeroed(2);
  for (int32 i = 0; i != tiles.Num() / 2; i++) {
    t[0] = tiles[2 * i]; t[1] = tiles[2 * i + 1];
    MovingTiles.Add(Cast<AMapState>(GetWorld()->GameState)->Map->GetTileFromIndex(t));
  }
}

bool AMapBasicEntity::MoveTo_Validate(const TArray<int32>& tiles)
{
  return true;
}
