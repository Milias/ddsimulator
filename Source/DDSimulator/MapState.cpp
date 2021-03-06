// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
#include <functional>
#include "MapState.h"

int32 FBattleData::BattleCount = 0;

AMapState::AMapState(const FObjectInitializer & PCIP) : Super(PCIP), Map(0)
{
  bReplicates = true;

  PowersDB = ConstructorHelpers::FObjectFinder<UPowerDataAsset>(TEXT("PowerDataAsset'/Game/DataBases/DB_Powers.DB_Powers'")).Object;
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

void AMapState::BeginMapBattle(const TArray<AMapBasicEntity*>& Entities)
{
  if (MapEntities.Num() == 0) { return; }
  if (Role < ROLE_Authority) {
    ServerBeginMapBattle(Entities);
  } else {
    DoBeginMapBattle(Entities);
  }
}

void AMapState::ServerBeginMapBattle_Implementation(const TArray<AMapBasicEntity*>& Entities)
{
  if (MapEntities.Num() == 0) { return; }
  DoBeginMapBattle(Entities);
}

bool AMapState::ServerBeginMapBattle_Validate(const TArray<AMapBasicEntity*>& Entities)
{
  return true;
}

void AMapState::DoBeginMapBattle(const TArray<AMapBasicEntity*>& Entities)
{
  MapBattles.Add(FBattleData(Entities));
  Entities[0]->EnterTurn();
}

void AMapState::NextTurn(AMapBasicEntity* Entity)
{
  if (!Entity->CombatAttributes.HasTurn) { return; }
  if (Role < ROLE_Authority) {
    ServerNextTurn(Entity);
  } else {
    DoNextTurn(Entity);
  }
}

void AMapState::ServerNextTurn_Implementation(AMapBasicEntity* Entity)
{
  if (!Entity->CombatAttributes.HasTurn) { return; }
  DoNextTurn(Entity);
}

bool AMapState::ServerNextTurn_Validate(AMapBasicEntity* Entity)
{
  return true;
}

void AMapState::DoNextTurn(AMapBasicEntity* Entity)
{
  FBattleData& BD = GetBattleDataByUID(Entity->InvolvedBattles);
  int32 ind = BD.InvolvedEntities.Find(Entity);
  int32 i = ind + 1;
  Entity->ExitTurn();
  while (i != ind) {
    if (i == BD.InvolvedEntities.Num()) { i = 0; }
    if (BD.InvolvedEntities[i]->CombatAttributes.CanEnterCombat) {
      BD.InvolvedEntities[i]->EnterTurn();
      return;
    } else { i++; }
  }
  BD.AllExitCombat();
}

void AMapState::UpdateBattleData()
{
  if (Role < ROLE_Authority) {
    ServerUpdateBattleData();
  } else {
    DoUpdateBattleData();
  }
}

void AMapState::ServerUpdateBattleData_Implementation()
{
  DoUpdateBattleData();
}

bool AMapState::ServerUpdateBattleData_Validate()
{
  return true;
}

void AMapState::DoUpdateBattleData()
{
  TArray<int32> inds;
  for (int32 i = 0; i != MapBattles.Num(); i++) {
    if (!MapBattles[i].CheckActive()) { inds.Add(i); }
  }
  for (int32 i = 0; i != inds.Num(); i++) { MapBattles.RemoveAt(inds[i]); }
}

FBattleData& AMapState::GetBattleDataByUID(int32 UID)
{
  return *MapBattles.FindByPredicate([&](const FBattleData& a) { return a.UID == UID; });
}

TArray<AMapBasicEntity*> AMapState::GetNextTurnEntities(int32 n)
{
  TArray<AMapBasicEntity*> R;
  for (int32 i = 0; i != n; i++) {
    R.Add(MapEntities[i%MapEntities.Num()]);
  }
  return R;
}

/*
Range: same as the one defined in "Power.h".
Center: origin tile.
Direction: for blasts,
  0 - Top
  1 - Top-right
  2 - Right
  3 - Down-right
  4 - Down
  5 - Down-left
  6 - Left
  7 - Top-left
*/
TArray<AMapTile*> AMapState::GetAreaTiles(const AMapBasicEntity* Entity, const TArray<int32>& Range, const AMapTile* Center, int32 EntitySize)
{
  TArray<AMapTile*> r; AMapTile* t = NULL;
  switch (Range[0]) {
  // Melee
  case 0:
    switch (Range[1]) {
    //Melee weapon
    case 0:
      break;

    //Melee touch
    case -1:
      break;

    //Melee #
    default:
      r.Append(GetTilesInRegion(Range[1], Entity->AssignedTiles[0], EntitySize));
    }
    break;

  //Ranged
  case 1:
    switch (Range[1]) {
    //Ranged weapon
    case 0:
      break;

    //Ranged sight
    case -1:
      break;

    //Ranged #
    default:
      r.Append(GetTilesInRegion(Range[1], Entity->AssignedTiles[0], EntitySize));
    }
    break;

  //Close
  case 2:
    switch (Range[1]) {
    //Burst
    case 0:
      r.Append(GetTilesInRegion(Range[2], Entity->AssignedTiles[0], EntitySize, false));
      break;

    //Blast
    case 1:
      break;
    }
    break;

  //AoE
  case 3:
    switch (Range[1]) {
    //AoE burst
    case 0:
      if (Entity->GetNearestAssignedTile(Center)->Index.MaxDist(Center->Index) > Range[3]) { break; }
      r.Append(GetTilesInRegion(Range[2], Center, 1));
      break;

    //AoE wall
    case 1:
      break;
    }
    break;
  }
  return r;
}

TArray<AMapTile*> AMapState::GetTilesInRegion(int32 Range, const AMapTile* Center, int32 EntitySize, bool IncludeCenter)
{
  TArray<AMapTile*> r; AMapTile* t = NULL;
  for (int32 i = 0; i != Range; i++) {
    for (int32 j = 0; j != Range; j++) {
      if (i == 0 && j == 0 && !IncludeCenter) { continue; }
      t = Map->UnsafeGetTileFromIndex(Center->Index + FTileIndex(i, j));
      if (t != NULL) { r.AddUnique(t); }
    }
  }
  for (int32 i = 1; i != EntitySize; i++) {
    for (int32 j = 0; j != Range + i; j++) {
      t = Map->UnsafeGetTileFromIndex(Center->Index + FTileIndex(j, Range + i));
      if (t != NULL) { r.AddUnique(t); }
      t = Map->UnsafeGetTileFromIndex(Center->Index + FTileIndex(Range + i, j));
      if (t != NULL) { r.AddUnique(t); }
    }
  }
  return r;
}

void AMapState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
  DOREPLIFETIME(AMapState, Map);
  DOREPLIFETIME(AMapState, MapEntities);
  DOREPLIFETIME(AMapState, MapBattles);
}