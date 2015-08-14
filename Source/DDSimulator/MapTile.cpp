// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
#include "MapTile.h"

AMapTile::AMapTile(const FObjectInitializer & PCIP) : Super(PCIP), Index(FTileIndex()), RelativePawnLocation(FVector(0, 0, 100))
{
  PrimaryActorTick.bCanEverTick = true;
  bReplicates = true;
  bAlwaysRelevant = true;
  AssignedEntity.SetNumUninitialized(0);
  OpenTiles.SetNumUninitialized(0);
  SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComponent"));
  Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
  Mesh->AttachTo(RootComponent);
  Mesh->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Meshes/Basic/BasicTile.BasicTile'")).Object);
}

FVector AMapTile::EntityPosition() { return GetActorLocation() + RelativePawnLocation; }

void AMapTile::AssignEntity_Implementation(AMapBasicEntity* ent)
{
  if (AssignedEntity.Contains(ent)) {
    AssignedEntity.Remove(ent);
  } else {
    AssignedEntity.Add(ent);
  }
}

bool AMapTile::AssignEntity_Validate(AMapBasicEntity* ent)
{
  return true;
}

void AMapTile::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
  DOREPLIFETIME(AMapTile, Index);
  DOREPLIFETIME(AMapTile, RelativePawnLocation);
  DOREPLIFETIME(AMapTile, AssignedEntity);
  DOREPLIFETIME(AMapTile, Mesh);
  DOREPLIFETIME(AMapTile, OpenTiles);
}

