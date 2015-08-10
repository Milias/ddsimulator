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
  TArray<UMapTile*> MapTiles;

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Map)
  USceneComponent * MapComponent;
  
  AMapBase(const FObjectInitializer & PCIP);
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

  UFUNCTION(BlueprintCallable, Category = Map)
  void CreateBasicGridMap(FVector Position, int32 MapWidth, int32 MapHeight);

  UFUNCTION(BlueprintCallable, Category = Map)
  UMapTile * GetTileFromPosition(FVector p);

  UFUNCTION(BlueprintCallable, Category = Map)
  UMapTile * GetTileFromIndex(TArray<int32> ind);

  UFUNCTION(BlueprintCallable, Category = Map)
  int32 DistInt(TArray<int32> v1, TArray<int32> v2) { return abs(v1[0] - v2[0]) + abs(v1[1] - v2[1]); }

  UFUNCTION(BlueprintCallable, Category = Map)
  int32 DistDD(TArray<int32> v1, TArray<int32> v2) { return std::max<int32>(abs(v1[0] - v2[0]), abs(v1[1] - v2[1])); }
};
