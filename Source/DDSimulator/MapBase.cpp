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

void AMapBase::CreateBasicGridMap_Implementation(FVector Position, int32 MapWidth, int32 MapHeight)
{
  if (MapWidth <= 0 || MapHeight <= 0) {
    UE_LOG(MapLog, Error, TEXT("Invalid map dimensions."));
    return;
  }
  MapComponent->SetRelativeLocation(Position);
  MapTiles.SetNumUninitialized(MapWidth*MapHeight);
  for (int32 i = 0; i < MapWidth; i++) {
    for (int32 j = 0; j < MapHeight; j++) {
      MapTiles[i + MapWidth*j] = GetWorld()->SpawnActor<AMapTile>();
      MapTiles[i + MapWidth*j]->Index(j, i);
      MapTiles[i + MapWidth*j]->AttachRootComponentTo(MapComponent, "MapTile");
      MapTiles[i + MapWidth*j]->SetActorLocation(FVector(j*TileHeight, i*TileWidth, 0));
      if (i==j && i == MapWidth/2) {
        MapTiles[i + MapWidth*j]->SetActorLocation(FVector(j*TileHeight, i*TileWidth, 100));
      }
    }
  }
  for (int32 i = 0; i < MapWidth; i++) {
    for (int32 j = 0; j < MapHeight; j++) {

      if (i != 0) { MapTiles[i + MapWidth*j]->OpenTiles.Add(MapTiles[i - 1 + MapWidth*j]); }
      if (i != MapWidth - 1) { MapTiles[i + MapWidth*j]->OpenTiles.Add(MapTiles[i + 1 + MapWidth*j]); }
      if (j != 0) { MapTiles[i + MapWidth*j]->OpenTiles.Add(MapTiles[i + MapWidth*(j - 1)]); }
      if (j != MapHeight - 1) { MapTiles[i + MapWidth*j]->OpenTiles.Add(MapTiles[i + MapWidth*(j + 1)]); }

      if (i != 0 && j != 0) { MapTiles[i + MapWidth*j]->OpenTiles.Add(MapTiles[i - 1 + MapWidth*(j - 1)]); }
      if (i != MapWidth - 1 && j != 0) { MapTiles[i + MapWidth*j]->OpenTiles.Add(MapTiles[i + 1 + MapWidth*(j-1)]); }
      if (j != MapHeight - 1 && i != MapWidth - 1) { MapTiles[i + MapWidth*j]->OpenTiles.Add(MapTiles[i + 1 + MapWidth*(j + 1)]); }
      if (i != 0 && j != MapHeight - 1) { MapTiles[i + MapWidth*j]->OpenTiles.Add(MapTiles[i - 1 + MapWidth*(j + 1)]); }
    }
  }
  CalculateClearance();
  UE_LOG(MapLog, Display, TEXT("Map created."));
}

bool AMapBase::CreateBasicGridMap_Validate(FVector Position, int32 MapWidth, int32 MapHeight)
{
  return true;
}

AMapTile * AMapBase::GetTileFromPosition(FVector p)
{
  int32 closest = 0; float t = FVector::DistSquared(p, MapTiles[0]->GetActorLocation());
  for (int32 i = 0; i < MapTiles.Num(); i++) {
    if (MapTiles[i]->GetActorLocation() == p) {
      closest = i; break;
    }
    if (FVector::DistSquared(p, MapTiles[i]->GetActorLocation()) < t) {
      closest = i;
      t = FVector::DistSquared(p, MapTiles[0]->GetActorLocation());
    }
  }
  return MapTiles[closest];
}

TArray<AMapTile*> AMapBase::GetTilesFromIndex(const TArray<FTileIndex>& pos)
{
  TArray<AMapTile*> R;
  for (int32 j = 0; j != pos.Num(); j++) {
    R.Add(GetTileFromIndex(pos[j]));
  }
  return R;
}

AMapTile* AMapBase::GetTileFromIndex(const FTileIndex& pos)
{
  int32 t; int32 closest = 0;
  t = MapTiles[0]->Index.AbsDist(pos);
  closest = 0;
  for (int32 i = 0; i < MapTiles.Num(); i++) {
    if (MapTiles[i]->Index == pos) {
      return MapTiles[i];
    }
    if (MapTiles[i]->Index.AbsDist(pos) < t) {
      closest = i; t = MapTiles[i]->Index.AbsDist(pos);
    }
  } return MapTiles[closest];
}

AMapTile* AMapBase::UnsafeGetTileFromIndex(const FTileIndex& Index)
{
  for (int32 i = 0; i < MapTiles.Num(); i++) {
    if (MapTiles[i]->Index == Index) {
      return MapTiles[i];
    }
  } return NULL;
}

bool AMapBase::CheckIndex(const FTileIndex& Index)
{
  for (int32 i = 0; i < MapTiles.Num(); i++) {
    if (MapTiles[i]->Index == Index) {
      return true;
    }
  } return false;
}

void AMapBase::CalculateClearance()
{
  if (Role < ROLE_Authority) {
    ServerCalculateClearance();
  } else {
    DoCalculateClearance();
  }
}

void AMapBase::ServerCalculateClearance_Implementation()
{
  DoCalculateClearance();
}

bool AMapBase::ServerCalculateClearance_Validate()
{
  return true;
}

void AMapBase::DoCalculateClearance()
{
  FTileIndex ind; bool stop;
  for (int32 tile = 0; tile != MapTiles.Num(); tile++) {
    if (!MapTiles[tile]->IsTransitable()) { continue; }
    stop = false;

    for (int32 i = 1; !stop; i++) {
      for (int32 j = 0; j < i; j++) {
        ind = MapTiles[tile]->Index + FTileIndex(i, j);
        if (CheckIndex(ind)) {
          if (!GetTileFromIndex(ind)->IsTransitable()) {
            stop = true; break;
          }
        } else { stop = true; break; }

        ind = MapTiles[tile]->Index + FTileIndex(j, i);
        if (CheckIndex(ind)) {
          if (!GetTileFromIndex(ind)->IsTransitable()) {
            stop = true; break;
          }
        } else { stop = true; break; }
      }

      ind = MapTiles[tile]->Index + FTileIndex(i, i);
      if (CheckIndex(ind) && !stop) { if (!GetTileFromIndex(ind)->IsTransitable()) { stop = true; } } else { stop = true; }

      MapTiles[tile]->Clearance = i;
    }
  }
}

void AMapBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
  DOREPLIFETIME(AMapBase, TileWidth);
  DOREPLIFETIME(AMapBase, TileHeight);
  DOREPLIFETIME(AMapBase, MapTiles);
}
