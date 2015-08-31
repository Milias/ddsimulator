// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
#include "MapHUD.h"

AMapHUD::AMapHUD() : MovingEntity(false) {}

void AMapHUD::DrawHUD()
{
  Super::DrawHUD();
}

void AMapHUD::UpdateSelection_Implementation(const TArray<AMapBasicEntity*>& ent) {}
