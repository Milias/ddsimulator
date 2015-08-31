// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "Power.h"
#include "PowerDataAsset.generated.h"

UCLASS()
class DDSIMULATOR_API UPowerDataAsset : public UDataAsset
{
  GENERATED_BODY()

public:

  UPROPERTY(EditAnywhere)
  TArray<FPowerData> items;

  UFUNCTION(BlueprintCallable, Category = Power)
  FPowerData& GetPowerByUID(int32 UID);

  UFUNCTION(BlueprintCallable, Category = Power)
  bool CheckExists(int32 UID);
};
