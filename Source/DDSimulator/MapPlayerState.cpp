// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
#include "MapPlayerState.h"

AMapPlayerState::AMapPlayerState(const FObjectInitializer & PCIP) : Super(PCIP)
{
}

void AMapPlayerState::Tick(float dt)
{
  Super::Tick(dt);
}

void AMapPlayerState::CastPower_Implementation(APower* Power, const TArray<AMapBasicEntity*>& EntityTarget, const TArray<AMapTile*>& TileTarget) 
{
  Power->PowerAction(EntityTarget, TileTarget);
}

bool AMapPlayerState::CastPower_Validate(APower* Power, const TArray<AMapBasicEntity*>& EntityTarget, const TArray<AMapTile*>& TileTarget) 
{
  return true;
}
