// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/PrimitiveComponent.h"
#include "MapBasicEntity.h"
#include "MapTile.generated.h"
                           
UCLASS()
class DDSIMULATOR_API UMapTile : public UStaticMeshComponent
{
  GENERATED_BODY()

public:
  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Map)
  TArray<int32> Index;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Entity)
  FVector RelativePawnLocation;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Entity)
  TArray<AMapBasicEntity*> AssignedEntity;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Map)
  TArray<UMapTile*> OpenTiles;

  UMapTile(const FObjectInitializer & PCIP);
  ~UMapTile() {}

  UFUNCTION(BlueprintCallable, Category = Map)
  void InitTile(int32 i, int32 j);

  UFUNCTION(BlueprintCallable, Category = Map)
  FVector EntityPosition();
};
