// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "GameFramework/GameNetworkManager.h"
#include "Matinee/MatineeActor.h"
#include "MapState.h"
#include "MapPlayerState.h"
#include "MapHUD.h"
#include "MapMode.generated.h"

UCLASS()
class DDSIMULATOR_API AMapMode : public AGameMode
{
  GENERATED_BODY()

public:
  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Entity)
  int32 EntityUID;

  AMapMode(const FObjectInitializer & PCIP);

  virtual void BeginPlay() override;
  virtual void Tick(float dt) override;

  UFUNCTION(BlueprintCallable, Category = Entity)
  AMapBase* CreateBaseMap();

  UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = Entity)
  void RegisterBasicEntity(const TArray<int32>& pos);

  UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = Entity)
  void UnRegisterBasicEntity(AMapBasicEntity * ent);

  //virtual void PreLogin(const FString & Options, const FString & Address, const TSharedPtr< class FUniqueNetId > & UniqueId, FString & ErrorMessage) override;
  //virtual void PostLogin(APlayerController * NewPlayer) override;
};
