// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
#include "MapBase.h"

AMapBase::AMapBase(const FObjectInitializer & PCIP) : Super(PCIP), TileWidth(260.0f), TileHeight(260.0f)
{
  PrimaryActorTick.bCanEverTick = false;
  bReplicates = true;
  bAlwaysRelevant = true;
  SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComponent"));
  MapComponent = CreateDefaultSubobject<USceneComponent>("MapCenter");
  MapComponent->AttachTo(RootComponent, "MapCenter");
  MapComponent->SetMobility(EComponentMobility::Movable);
}

void AMapBase::BeginPlay()
{
  Super::BeginPlay();
}

void AMapBase::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
}

void AMapBase::CreateBasicGridMap(FVector Position, int32 MapWidth, int32 MapHeight)
{
  if (MapWidth <= 0 || MapHeight <= 0) {
    UE_LOG(MapLog, Error, TEXT("Invalid map dimensions."));
    return;
  }
  RootComponent->SetWorldLocation(Position);
  MapTiles.SetNumUninitialized(MapWidth*MapHeight);
  for (int32 i = 0; i < MapWidth; i++) {
    for (int32 j = 0; j < MapHeight; j++) {
      MapTiles[i + MapWidth*j] = NewObject<UMapTile>(this);
      MapTiles[i + MapWidth*j]->InitTile(i, j);
      MapTiles[i + MapWidth*j]->AttachTo(MapComponent);
      MapTiles[i + MapWidth*j]->SetMobility(EComponentMobility::Movable);
      MapTiles[i + MapWidth*j]->SetRelativeLocation(FVector(i*TileWidth, j*TileHeight, 0));
      MapTiles[i + MapWidth*j]->RegisterComponent();
    }
  }
  for (int32 i = 0; i < MapWidth; i++) {
    for (int32 j = 0; j < MapHeight; j++) {
      if (i != 0) { MapTiles[i + MapWidth*j]->OpenTiles.Add(MapTiles[i - 1 + MapWidth*j]); }
      if (i != MapWidth - 1) { MapTiles[i + MapWidth*j]->OpenTiles.Add(MapTiles[i + 1 + MapWidth*j]); }
      if (j != 0) { MapTiles[i + MapWidth*j]->OpenTiles.Add(MapTiles[i + MapWidth*(j - 1)]); }
      if (j != MapHeight - 1) { MapTiles[i + MapWidth*j]->OpenTiles.Add(MapTiles[i + MapWidth*(j + 1)]); }
    }
  }
  UE_LOG(MapLog, Display, TEXT("Map created."));
}

UMapTile * AMapBase::GetTileFromPosition(FVector p)
{
  int32 closest = 0; float t = FVector::DistSquared(p, MapTiles[0]->GetComponentLocation());
  for (int32 i = 0; i < MapTiles.Num(); i++) {
    if (MapTiles[i]->GetComponentLocation() == p) {
      closest = i; break;
    }
    if (FVector::DistSquared(p, MapTiles[i]->GetComponentLocation()) < t) {
      closest = i;
      t = FVector::DistSquared(p, MapTiles[0]->GetComponentLocation());
    }
  }
  return MapTiles[closest];
}

UMapTile * AMapBase::GetTileFromIndex(TArray<int32> ind)
{
  int32 t = DistInt(MapTiles[0]->Index,ind);
  int32 closest = 0;
  for (int32 i = 0; i < MapTiles.Num(); i++) {
    if (MapTiles[i]->Index == ind) {
      return MapTiles[i];
    }
    if (DistInt(MapTiles[i]->Index, ind) < t) {
      closest = i; t = DistInt(MapTiles[i]->Index, ind);
    }
  }
  return MapTiles[closest];
}

void AMapBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
  DOREPLIFETIME(AMapBase, TileWidth);
  DOREPLIFETIME(AMapBase, TileHeight);
  DOREPLIFETIME(AMapBase, MapTiles);
}
