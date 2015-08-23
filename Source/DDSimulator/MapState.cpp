// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
#include <functional>
#include "MapState.h"

AMapState::AMapState(const FObjectInitializer & PCIP) : Super(PCIP), Map(0)
{
  bReplicates = true;
}

void AMapState::BeginPlay()
{
  Super::BeginPlay();
}

void AMapState::Tick(float dt)
{
  Super::Tick(dt);
}

TArray<AMapBasicEntity*> AMapState::GetBasicEntityByIndex(const TArray<FTileIndex>& pos)
{
  TArray<AMapBasicEntity*> ret;
  for (int32 i = 0; i != pos.Num(); i++) {
    ret.Append(Map->GetTileFromIndex(pos[i])->AssignedEntity);
  }
  return ret;
}

TArray<AMapBasicEntity*> AMapState::GetBasicEntityByUID(int32 uid)
{
  TArray<AMapBasicEntity*> ret;
  for (auto it = MapEntities.CreateConstIterator(); it; ++it) {
    if ((*it)->uid == uid) { ret.Add(*it); }
  }
  return ret;
}

void AMapState::SortEntitiesByInitiative()
{
  if (MapEntities.Num() == 0) { return; }
  if (Role < ROLE_Authority) {
    ServerSortEntitiesByInitiative();
  } else {
    DoSortEntitiesByInitiative();
  }
}

void AMapState::ServerSortEntitiesByInitiative_Implementation()
{
  if (MapEntities.Num() == 0) { return; }
  DoSortEntitiesByInitiative();
}

bool AMapState::ServerSortEntitiesByInitiative_Validate()
{
  return true;
}

void AMapState::DoSortEntitiesByInitiative()
{
  
}

void AMapState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
  DOREPLIFETIME(AMapState, Map);
  DOREPLIFETIME(AMapState, MapEntities);
}