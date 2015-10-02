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

TArray<AMapTile*> AMapState::GetTilesInRegion(int32 Range, const AMapTile* Center, int32 EntitySize, bool IncludeCenter)
{
  TArray<AMapTile*> r; AMapTile* t = NULL;
  for (int32 i = -Range; i <= Range; i++) {
    for (int32 j = -Range; j <= Range; j++) {
      if (!IncludeCenter && (i < EntitySize && i >= 0 && j < EntitySize && j >= 0)) { continue; }
      t = Map->UnsafeGetTileFromIndex(Center->Index + FTileIndex(i, j));
      if (t != NULL) { r.AddUnique(t); }
    }
  }
  for (int32 i = 1; i < EntitySize; i++) {
    for (int32 j = -Range; j <= Range + i; j++) {
      t = Map->UnsafeGetTileFromIndex(Center->Index + FTileIndex(j, Range + i));
      if (t != NULL) { r.AddUnique(t); }
      t = Map->UnsafeGetTileFromIndex(Center->Index + FTileIndex(Range + i, j));
      if (t != NULL) { r.AddUnique(t); }
    }
  }
  return r;
}

TArray<AMapTile*> AMapState::GetTilesInRegion(int32 Range, AMapBasicEntity * Entity, bool IncludeCenter)
{
  TArray<AMapTile*> r; AMapTile* t = NULL;
  for (int32 i = -Range; i <= Range; i++) {
    for (int32 j = -Range; j <= Range; j++) {
      if (!IncludeCenter && (i < Entity->EntitySize && i >= 0 && j < Entity->EntitySize && j >= 0)) { continue; }
      t = Map->UnsafeGetTileFromIndex(Entity->AssignedTiles[0]->Index + FTileIndex(i, j));
      if (t != NULL) {
        if (!BresenhamEntityCheck(Entity, t)) { r.Empty(); return r; }
        r.AddUnique(t);
      }
    }
  }
  for (int32 i = 1; i < Entity->EntitySize; i++) {
    for (int32 j = -Range; j <= Range + i; j++) {
      t = Map->UnsafeGetTileFromIndex(Entity->AssignedTiles[0]->Index + FTileIndex(j, Range + i));
      if (t != NULL) {
        if (!BresenhamEntityCheck(Entity, t)) { r.Empty(); return r; }
        r.AddUnique(t);
      }
      t = Map->UnsafeGetTileFromIndex(Entity->AssignedTiles[0]->Index + FTileIndex(Range + i, j));
      if (t != NULL) {
        if (!BresenhamEntityCheck(Entity, t)) { r.Empty(); return r; }
        r.AddUnique(t);
      }
    }
  }
  return r;
}

AMapTile * AMapState::Bresenham(AMapTile* Center, AMapTile * Destination, int32 Distance, BresenhamLineType Type)
{
  AMapTile * loop = NULL;
  FTileIndex p = Center->Index;
  FTileIndex d = Destination->Index - Center->Index;
  FVector2D p2(p.X, p.Y);
  float N = d.Max(); if (N == 0) return Center;
  FTileIndex t = p2;
  float sx = d.X / N; float sy = d.Y / N;
  for (int32 i = 0; i < N && (Center->Index.MaxDist(FTileIndex(p2)) < Distance || Distance == -1) && Map->CheckIndex(FTileIndex(p2)); i++) {
    print(FString::FromInt(p2.X) + " " + FString::FromInt(p2.Y));
    loop = Map->UnsafeGetTileFromIndex(FTileIndex(p2));
    if (loop == NULL) {
      return Map->GetTileFromIndex(t);
    } else {
      if (!BresenhamLineCheck(loop, Type)) { return Map->GetTileFromIndex(t); }
      t = p2;
      p2.X += sx;
      p2.Y += sy;
    }
  }
  return loop;
}

bool AMapState::BresenhamCheck(AMapTile* Center, AMapTile * Destination, int32 Distance, BresenhamLineType Type)
{
  AMapTile * loop = NULL;
  FTileIndex p = Center->Index;
  FTileIndex d = Destination->Index - Center->Index;
  FVector2D p2(p.X, p.Y);
  float N = d.Max(); if (N == 0) return true;
  FTileIndex t = p2;
  float sx = d.X / N; float sy = d.Y / N;
  for (int32 i = 0; i < N && (Center->Index.MaxDist(FTileIndex(p2)) < Distance || Distance == -1); i++) {
    loop = Map->UnsafeGetTileFromIndex(FTileIndex(p2));
    if (loop == NULL) {
      return Destination->Index == t;
    } else {
      if (!BresenhamLineCheck(loop, Type)) { return Destination->Index == t; }
      t = p2;
      p2.X += sx;
      p2.Y += sy;
    }
  }
  return Destination->Index == loop->Index;
}

AMapTile * AMapState::BresenhamEntity(AMapBasicEntity * Entity, AMapTile * Destination, int32 MinDistance, int32 MaxDistance, BresenhamLineType Type)
{
  AMapTile * r = Entity->GetMiddleTile(); float dist = r->Index.SqrDist(Destination->Index); AMapTile * t = NULL; float d = 0;
  for (int32 i = 0; i < Entity->AssignedTiles.Num(); i++) {
    t = Bresenham(Entity->AssignedTiles[i], Destination, MaxDistance, Type); d = t->Index.SqrDist(Destination->Index);
    if (MinDistance != -1) {
      for (int32 j = 0; j < Entity->AssignedTiles.Num(); j++) {
        if (t->Index.MaxDist(Entity->AssignedTiles[i]->Index) < MinDistance) { return NULL; }
      }
    }
    if (d < dist) { r = t; dist = d; }
  }
  return r;
}

bool AMapState::BresenhamEntityCheck(AMapBasicEntity * Entity, AMapTile * Destination, int32 MaxDistance, BresenhamLineType Type)
{
  for (int32 i = 0; i < Entity->AssignedTiles.Num(); i++) {
    if (!BresenhamCheck(Entity->AssignedTiles[i], Destination, MaxDistance, Type)) return false;
  }
  return true;
}

bool AMapState::BresenhamLineCheck(AMapTile * Tile, BresenhamLineType Type)
{
  if (Type == Nothing) {
    return true;
  } else if (Type == Transitable) {
    return Tile->IsTransitable();
  } else if (Type == LineOfSight) {
    return Tile->LineOfSight;
  } else if (Type == LineOfEffect) {
    return Tile->LineOfEffect;
  }
  return false;
}

void AMapState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
  DOREPLIFETIME(AMapState, Map);
  DOREPLIFETIME(AMapState, MapEntities);
  DOREPLIFETIME(AMapState, MapBattles);
}