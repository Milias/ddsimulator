// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/PrimitiveComponent.h"
#include "MapBasicEntity.h"
#include "MapTile.generated.h"

UCLASS()
class DDSIMULATOR_API AMapTile : public AActor
{
  GENERATED_BODY()

public:
  UPROPERTY(Replicated, BlueprintReadOnly, VisibleAnywhere, Category = Map)
  FTileIndex Index;

  UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Entity)
  UStaticMeshComponent * Mesh;

  UPROPERTY(Replicated, BlueprintReadOnly, VisibleAnywhere, Category = Entity)
  FVector RelativePawnLocation;

  UPROPERTY(Replicated, BlueprintReadOnly, VisibleAnywhere, Category = Entity)
  TArray<AMapBasicEntity*> AssignedEntity;

  UPROPERTY(Replicated, BlueprintReadOnly, VisibleAnywhere, Category = Map)
  TArray<AMapTile*> OpenTiles;

  AMapTile(const FObjectInitializer & PCIP);

  UFUNCTION(BlueprintCallable, Category = Entity)
  FVector EntityPosition();

  /*
    If this tile already contains ent, AssignEntity will remove it.
    Otherwise, ent will be Add-ed to AssignedEntity.
  */
  UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = Entity)
  void AssignEntity(AMapBasicEntity* ent);
};
