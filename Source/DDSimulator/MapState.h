// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "MapBase.h"
#include "MapState.generated.h"

UCLASS()
class DDSIMULATOR_API AMapState : public AGameState
{
	GENERATED_BODY()

public:
  
  UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Map)
  AMapBase * Map;

  UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Entity)
  TArray<AMapBasicEntity*> MapEntities;

  UPROPERTY(/*Replicated*/)
  TMap<int32, UMapTile*> MapEntityTiles;
  
  AMapState(const FObjectInitializer & PCIP);

  virtual void BeginPlay() override;
  virtual void Tick(float dt) override;

  UFUNCTION(BlueprintCallable, Category = Entity)
  TArray<AMapBasicEntity*> GetBasicEntityByIndex(TArray<int32> pos);

  UFUNCTION(BlueprintCallable, Category = Entity)
  AMapBasicEntity* GetBasicEntityByUID(int32 uid);
};
