// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
#include "MapMode.h"

AMapMode::AMapMode(const FObjectInitializer & PCIP) : Super(PCIP)
{
  PrimaryActorTick.bCanEverTick = false;
  GameStateClass = AMapState::StaticClass();
  PlayerStateClass = AMapPlayerState::StaticClass();
  DefaultPawnClass = AMapPlayerPawn::StaticClass();
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

void AMapMode::RegisterBasicEntity_Implementation(const TArray<FTileIndex>& pos)
{
  AMapBasicEntity* ent;
  ent = GetWorld()->SpawnActor<AMapBasicEntity>(AMapBasicEntity::StaticClass());
  ent->AssignTiles(Cast<AMapState>(GameState)->Map->GetTilesFromIndex(pos));

  Cast<AMapState>(GameState)->MapEntities.Add(ent);
}

bool AMapMode::RegisterBasicEntity_Validate(const TArray<FTileIndex>& pos)
{
  return true;
}

void AMapMode::UnRegisterBasicEntity_Implementation(AMapBasicEntity * ent)
{
  ent->UnRegister();
  Cast<AMapState>(GameState)->MapEntities.Remove(ent);
  ent->Destroy();
}

bool AMapMode::UnRegisterBasicEntity_Validate(AMapBasicEntity * ent)
{
  return true;
}
