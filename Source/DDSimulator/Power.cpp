// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
#include "MapState.h"
#include "Power.h"

APower::APower() : OwnerEntity(NULL), IsInitialized(false)
{
  bReplicates = true;
  bAlwaysRelevant = true;
	PrimaryActorTick.bCanEverTick = false;
}

void APower::SetOwnerEntity(AMapBasicEntity * Entity)
{
  if (OwnerEntity != NULL || GetOwner() != NULL) { return; }
  if (Role < ROLE_Authority) {
    ServerSetOwnerEntity(Entity);
  } else {
    DoSetOwnerEntity(Entity);
  }
}

void APower::ServerSetOwnerEntity_Implementation(AMapBasicEntity * Entity)
{
  if (OwnerEntity != NULL || GetOwner() != NULL) { return; }
  DoSetOwnerEntity(Entity);
}

bool APower::ServerSetOwnerEntity_Validate(AMapBasicEntity * Entity)
{
  return true;
}

void APower::DoSetOwnerEntity(AMapBasicEntity * Entity)
{
  OwnerEntity = Entity;
}

void APower::Initialize(int32 UID)
{
  if (Cast<AMapState>(GetWorld()->GameState) == NULL && !IsInitialized) { return; }
  if (!Cast<AMapState>(GetWorld()->GameState)->PowersDB->CheckExists(UID)) { return; }
  if (Role < ROLE_Authority) {
    ServerInitialize(UID);
  } else {
    DoInitialize(UID);
  }
}

void APower::ServerInitialize_Implementation(int32 UID)
{
  if (Cast<AMapState>(GetWorld()->GameState) == NULL && !IsInitialized) { return; }
  if (!Cast<AMapState>(GetWorld()->GameState)->PowersDB->CheckExists(UID)) { return; }
  DoInitialize(UID);
}

bool APower::ServerInitialize_Validate(int32 UID)
{
  return true;
}

void APower::DoInitialize(int32 UID)
{
  Data = Cast<AMapState>(GetWorld()->GameState)->PowersDB->GetPowerByUID(UID);
  IsInitialized = true;
}

void APower::PowerAction(const TArray<AMapBasicEntity*>& Targets, const TArray<AMapTile*>& TileTarget)
{
  if (!IsInitialized) { return; }
  print(FString::FromInt(Role));
  if (Role < ROLE_Authority) {
    ServerPowerAction(Targets, TileTarget);
  } else {
    DoPowerAction(Targets, TileTarget);
  }
}

void APower::ServerPowerAction_Implementation(const TArray<AMapBasicEntity*>& Targets, const TArray<AMapTile*>& TileTarget)
{
  if (!IsInitialized) { return; }
  DoPowerAction(Targets, TileTarget);
}

bool APower::ServerPowerAction_Validate(const TArray<AMapBasicEntity*>& Targets, const TArray<AMapTile*>& TileTarget)
{
  return false;
}

void APower::DoPowerAction_Implementation(const TArray<AMapBasicEntity*>& Targets, const TArray<AMapTile*>& TileTarget)
{
  print("Power " + Data.Name + " casted on " + FString::FromInt(Targets.Num()) + " targets.");
}

void APower::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
  DOREPLIFETIME(APower, OwnerEntity);
  DOREPLIFETIME(APower, Data);
  DOREPLIFETIME(APower, IsInitialized);
}
