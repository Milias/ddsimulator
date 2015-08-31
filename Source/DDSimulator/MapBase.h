// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MapTile.h"
#include "MapBase.generated.h"

UCLASS()
class DDSIMULATOR_API AMapBase : public AActor
{
	GENERATED_BODY()
	
public:
  UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = Map)
  float TileWidth;

  UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = Map)
  float TileHeight;
  
  UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = Map)
  TArray<AMapTile*> MapTiles;

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Map)
  USceneComponent * MapComponent;
  
  AMapBase(const FObjectInitializer & PCIP);
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

  UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = Map)
  void CreateBasicGridMap(FVector Position, int32 MapWidth, int32 MapHeight);

  UFUNCTION(BlueprintCallable, Category = Map)
  AMapTile * GetTileFromPosition(FVector p);

  UFUNCTION(BlueprintCallable, Category = Map)
  TArray<AMapTile*> GetTilesFromIndex(const TArray<FTileIndex>& Index);

  UFUNCTION(BlueprintCallable, Category = Map)
  AMapTile* GetTileFromIndex(const FTileIndex& Index);

  UFUNCTION(BlueprintCallable, Category = Map)
  AMapTile* UnsafeGetTileFromIndex(const FTileIndex& Index);

  UFUNCTION(BlueprintCallable, Category = Map)
  bool CheckIndex(const FTileIndex& Index);

  UFUNCTION(BlueprintCallable, Category = Map)
  void CalculateClearance();

  UFUNCTION(Server, WithValidation, Reliable)
  void ServerCalculateClearance();

  void DoCalculateClearance();
};
