// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
#include "MapTile.h"

UMapTile::UMapTile(const FObjectInitializer & PCIP) : Super(PCIP), RelativePawnLocation(FVector(0, 0, 100))
{
  bReplicates = true;
  Index.SetNumZeroed(2);
  AssignedEntity.SetNumUninitialized(0);
  OpenTiles.SetNumUninitialized(0);
  SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Meshes/Basic/BasicTile.BasicTile'")).Object);
}

void UMapTile::InitTile(int32 i, int32 j) { Index[0] = i; Index[1] = j; }

FVector UMapTile::EntityPosition() { return GetComponentLocation() + RelativePawnLocation; }
/*
void UMapTile::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
  DOREPLIFETIME(UMapTile, Index);
  DOREPLIFETIME(UMapTile, RelativePawnLocation);
  DOREPLIFETIME(UMapTile, AssignedEntity);
  DOREPLIFETIME(UMapTile, OpenTiles);
}
*/