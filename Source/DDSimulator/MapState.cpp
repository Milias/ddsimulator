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
  //DOREPLIFETIME(AMapState, MapEntityTiles);
}

TArray<AMapBasicEntity*> AMapState::GetBasicEntityByIndex(TArray<int32> pos)
{
  TArray<AMapBasicEntity*> ret;
  if (pos.Num() == 2) {
     ret.Append(Map->GetTileFromIndex(pos)->AssignedEntity);
  } else {
    TArray<int32> t; t.SetNumZeroed(2);
    for (int32 i = 0; i != pos.Num() / 2; i++) {
      t[0] = pos[2 * i]; t[1] = pos[2 * i + 1];
      ret.Append(Map->GetTileFromIndex(t)->AssignedEntity);
    }
  }
  return ret;
}

/*
  Check if returned pointer is NULL.
*/
AMapBasicEntity* AMapState::GetBasicEntityByUID(int32 uid)
{
  AMapBasicEntity * ret = 0;
  for (auto it = MapEntities.CreateConstIterator(); it; ++it) {
    if ((*it)->uid == uid) { ret = *it; break; }
  }
  return ret;
}
