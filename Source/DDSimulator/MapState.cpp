// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
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

void AMapState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
  DOREPLIFETIME(AMapState, Map);
  DOREPLIFETIME(AMapState, MapEntities);
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
