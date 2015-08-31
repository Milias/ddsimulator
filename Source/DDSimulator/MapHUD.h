// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "MapHUD.generated.h"

UCLASS()
class DDSIMULATOR_API AMapHUD : public AHUD
{
	GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
  bool MovingEntity;

  AMapHUD();

  virtual void DrawHUD() override;

  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintCosmetic, Category = Selection)
  void UpdateSelection(const TArray<AMapBasicEntity*>& SelectedEntities);
};

