// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "MapHUD.generated.h"

UCLASS()
class DDSIMULATOR_API AMapHUD : public AHUD
{
	GENERATED_BODY()

public:
  AMapHUD();

  virtual void DrawHUD() override;
};
