// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
#include "MapPlayerState.h"

AMapPlayerState::AMapPlayerState(const FObjectInitializer & PCIP) : Super(PCIP)
{
  SelectedEntity.SetNumUninitialized(0);
}

void AMapPlayerState::Tick(float dt)
{
  Super::Tick(dt);
}
