// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "GameFramework/GameNetworkManager.h"
#include "Matinee/MatineeActor.h"
#include "MapState.h"
#include "MapMode.generated.h"

UCLASS()
class DDSIMULATOR_API AMapMode : public AGameMode
{
  GENERATED_BODY()

public:
  AMapMode(const FObjectInitializer & PCIP);

  virtual void BeginPlay() override;
  virtual void Tick(float dt) override;

  UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = Map)
  void ServerCreateBaseMap();
};
