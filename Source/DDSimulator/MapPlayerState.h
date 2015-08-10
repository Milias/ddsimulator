// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "MapPlayerController.h"
#include "MapBasicEntity.h"
#include "MapPlayerState.generated.h"

UCLASS()
class DDSIMULATOR_API AMapPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Selection)
  TArray<AMapBasicEntity*> SelectedEntity;

  AMapPlayerState(const FObjectInitializer & PCIP);

  virtual void Tick(float dt) override;
};
