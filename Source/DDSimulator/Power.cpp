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
  if (OwnerEntity != NULL) { return; }
  if (Role < ROLE_Authority) {
    ServerSetOwnerEntity(Entity);
  } else {
    DoSetOwnerEntity(Entity);
  }
}

void APower::ServerSetOwnerEntity_Implementation(AMapBasicEntity * Entity)
{
  if (OwnerEntity != NULL) { return; }
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
  print(Data.Name);
  IsInitialized = true;
}

void APower::PowerAction(const TArray<AMapBasicEntity*>& Targets)
{
  if (!IsInitialized) { return; }
  if (Role < ROLE_Authority) {
    ServerPowerAction(Targets);
  } else {
    DoPowerAction(Targets);
  }
}

void APower::ServerPowerAction_Implementation(const TArray<AMapBasicEntity*>& Targets)
{
  if (!IsInitialized) { return; }
  DoPowerAction(Targets);
}

bool APower::ServerPowerAction_Validate(const TArray<AMapBasicEntity*>& Targets)
{
  return true;
}

void APower::DoPowerAction_Implementation(const TArray<AMapBasicEntity*>& Targets)
{
  print("Power " + Data.Name + " launched.");
}

void APower::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
  DOREPLIFETIME(APower, OwnerEntity);
  DOREPLIFETIME(APower, Data);
  DOREPLIFETIME(APower, IsInitialized);
}
