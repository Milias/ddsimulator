// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
#include "MapTile.h"

AMapTile::AMapTile(const FObjectInitializer & PCIP) : Super(PCIP), Index(FTileIndex()), RelativePawnLocation(FVector(0, 0, 100)), Clearance(0), Transitable(true), LineOfSight(true), LineOfEffect(true)
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
  Mesh->SetCollisionResponseToChannel(COLLISION_MAP_TILE, ECollisionResponse::ECR_Block);
}

FVector AMapTile::EntityPosition() { return GetActorLocation() + RelativePawnLocation; }

void AMapTile::AssignEntity(AMapBasicEntity* ent)
{
  if (Role < ROLE_Authority) {
    ServerAssignEntity(ent);
  } else {
    DoAssignEntity(ent);
  }
}

void AMapTile::ServerAssignEntity_Implementation(AMapBasicEntity* ent)
{
  DoAssignEntity(ent);
}

bool AMapTile::ServerAssignEntity_Validate(AMapBasicEntity* ent)
{
  return true;
}

void AMapTile::DoAssignEntity(AMapBasicEntity* ent)
{
  if (AssignedEntity.Contains(ent)) {
    AssignedEntity.Remove(ent);
  } else {
    AssignedEntity.Add(ent);
  }
}

bool AMapTile::IsTransitable()
{
  return Transitable;
}

void AMapTile::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
  DOREPLIFETIME(AMapTile, Index);
  DOREPLIFETIME(AMapTile, RelativePawnLocation);
  DOREPLIFETIME(AMapTile, AssignedEntity);
  DOREPLIFETIME(AMapTile, Mesh);
  DOREPLIFETIME(AMapTile, OpenTiles);
  DOREPLIFETIME(AMapTile, Clearance);
  DOREPLIFETIME(AMapTile, Transitable);
  DOREPLIFETIME(AMapTile, LineOfSight);
  DOREPLIFETIME(AMapTile, LineOfEffect);
}
