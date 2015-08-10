// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
#include "MapMode.h"

AMapMode::AMapMode(const FObjectInitializer & PCIP) : Super(PCIP), EntityUID(0)
{
  PrimaryActorTick.bCanEverTick = false;
  GameStateClass = AMapState::StaticClass();
  PlayerStateClass = AMapPlayerState::StaticClass();
  PlayerControllerClass = AMapPlayerController::StaticClass();
  DefaultPawnClass = AMapPlayerPawn::StaticClass();
  HUDClass = AMapHUD::StaticClass();
}

void AMapMode::BeginPlay()
{
  Super::BeginPlay();
}

void AMapMode::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
}

AMapBase* AMapMode::CreateBaseMap()
{
  Cast<AMapState>(GameState)->Map = GetWorld()->SpawnActor<AMapBase>(AMapBase::StaticClass());
  return Cast<AMapState>(GameState)->Map;
}

void AMapMode::RegisterBasicEntity_Implementation(const TArray<int32>& pos)
{
  TArray<UMapTile*> tiles; AMapBasicEntity* ent; TArray<int32> temp; FVector loc;
  ent = GetWorld()->SpawnActor<AMapBasicEntity>(AMapBasicEntity::StaticClass());

  if (pos.Num() == 2) {
    tiles.Add(Cast<AMapState>(GameState)->Map->GetTileFromIndex(pos));
    loc = tiles.Last()->GetComponentLocation() + tiles.Last()->RelativePawnLocation;
    tiles.Last()->AssignedEntity.Add(ent);
  } else {
    temp.SetNumZeroed(2); loc = FVector::ZeroVector;
    for (int32 i = 0; i != pos.Num() / 2; i++) {
      temp[0] = pos[2 * i]; temp[1] = pos[2 * i + 1];
      tiles.Add(Cast<AMapState>(GameState)->Map->GetTileFromIndex(temp));
      loc = loc + tiles.Last()->GetComponentLocation() + tiles.Last()->RelativePawnLocation;
      tiles.Last()->AssignedEntity.Add(ent);
    }
    loc = loc / tiles.Num();
  }

  ent->SetActorLocation(loc);
  ent->uid = EntityUID++;

  Cast<AMapState>(GameState)->MapEntities.Add(ent);
  for (auto it = tiles.CreateConstIterator(); it; ++it) {
    Cast<AMapState>(GameState)->MapEntityTiles.Add(ent->uid, *it);
  }
}

bool AMapMode::RegisterBasicEntity_Validate(const TArray<int32>& pos)
{
  return true;
}

void AMapMode::UnRegisterBasicEntity_Implementation(AMapBasicEntity * ent)
{
  // Creates an iterator over the elements of the map with key 'ent->uid'.
  for (auto it = Cast<AMapState>(GameState)->MapEntityTiles.CreateConstKeyIterator(ent->uid); it; ++it) {
    it.Value()->AssignedEntity.Remove(ent);
  }
  Cast<AMapState>(GameState)->MapEntities.Remove(ent);
  ent->Destroy();
}

bool AMapMode::UnRegisterBasicEntity_Validate(AMapBasicEntity * ent)
{
  return true;
}
