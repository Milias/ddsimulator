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

void AMapMode::ServerCreateBaseMap_Implementation()
{
  Cast<AMapState>(GameState)->Map = GetWorld()->SpawnActor<AMapBase>(AMapBase::StaticClass());
}

bool AMapMode::ServerCreateBaseMap_Validate()
{
  return true;
}

void AMapMode::ServerBeginMapBattle_Implementation()
{
  AMapState * state = Cast<AMapState>(GameState);
  if (state->MapEntities.Num() == 0) { return; }

}

bool AMapMode::ServerBeginMapBattle_Validate()
{
  return true;
}

