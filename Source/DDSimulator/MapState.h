// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "MapBase.h"
#include "MapPlayerState.h"
#include "MapState.generated.h"

USTRUCT()
struct FBattleData {
  GENERATED_USTRUCT_BODY()

  static int32 BattleCount;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  int32 UID;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  bool IsActive;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  TArray<AMapBasicEntity*> InvolvedEntities;

  void SortByInitiative() {
    InvolvedEntities.Sort([](const AMapBasicEntity& a, const AMapBasicEntity& b) {
      return a.CombatAttributes.Initiative[0] > b.CombatAttributes.Initiative[0];
    });
  }

  void FinishBattle() {
    if (InvolvedEntities.Num() > 0) { InvolvedEntities.Empty(); }
    IsActive = false;
  }

  void AllExitCombat() {
    for (int32 i = 0; i != InvolvedEntities.Num(); i++) {
      InvolvedEntities[i]->InvolvedBattles = -1;
      InvolvedEntities[i]->ExitCombat();
    }
  }

  bool CheckActive() const {
    if (!IsActive) { return false; }
    int32 EntitiesNotInBattle = 0;
    for (int32 i = 0; i != InvolvedEntities.Num(); i++) {
      if (InvolvedEntities[i]->InvolvedBattles != UID) { EntitiesNotInBattle++; }
    }
    return EntitiesNotInBattle != InvolvedEntities.Num();
  }

  void UpdateInvolvedEntities() {
    for (int32 i = 0; i != InvolvedEntities.Num(); i++) {
      if (InvolvedEntities[i]->InvolvedBattles == -1) {
        InvolvedEntities[i]->EnterCombat();
      }
      InvolvedEntities[i]->InvolvedBattles = UID;
    }
    SortByInitiative();
  }

  FBattleData& operator+=(FBattleData& a) {
    InvolvedEntities.Append(a.InvolvedEntities);
    UpdateInvolvedEntities();
    a.FinishBattle();
    return *this;
  }

  FBattleData& operator+=(AMapBasicEntity*& e) {
    InvolvedEntities.Add(e);
    UpdateInvolvedEntities();
    return *this;
  }

  FBattleData& operator+=(const TArray<AMapBasicEntity*>& e) {
    InvolvedEntities.Append(e);
    UpdateInvolvedEntities();
    return *this;
  }

  FBattleData() { UID = BattleCount++; IsActive = true; }
  FBattleData(const TArray<AMapBasicEntity*>& Entities) { UID = BattleCount++; *this += Entities;  IsActive = true; }
};

UCLASS()
class DDSIMULATOR_API AMapState : public AGameState
{
	GENERATED_BODY()

public:
  UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Map)
  AMapBase * Map;

  UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Entity)
  TArray<AMapBasicEntity*> MapEntities;
  
  UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Combat)
  TArray<FBattleData> MapBattles;
  
  AMapState(const FObjectInitializer & PCIP);

  virtual void BeginPlay() override;
  virtual void Tick(float dt) override;

  UFUNCTION(BlueprintCallable, Category = Entity)
  TArray<AMapBasicEntity*> GetBasicEntityByIndex(const TArray<FTileIndex>& pos);

  /* Check if returned array is empty. */
  UFUNCTION(BlueprintCallable, Category = Entity)
  TArray<AMapBasicEntity*> GetBasicEntityByUID(int32 uid);

  UFUNCTION(BlueprintCallable, Category = Combat)
  void BeginMapBattle(const TArray<AMapBasicEntity*>& Entities);

  UFUNCTION(Server, WithValidation, Reliable)
  void ServerBeginMapBattle(const TArray<AMapBasicEntity*>& Entities);

  void DoBeginMapBattle(const TArray<AMapBasicEntity*>& Entities);

  UFUNCTION(BlueprintCallable, Category = Combat)
  void NextTurn(AMapBasicEntity* Entity);

  UFUNCTION(Server, WithValidation, Reliable)
  void ServerNextTurn(AMapBasicEntity* Entity);

  void DoNextTurn(AMapBasicEntity* Entity);

  UFUNCTION(BlueprintCallable, Category = Combat)
  void UpdateBattleData();

  UFUNCTION(Server, WithValidation, Reliable)
  void ServerUpdateBattleData();

  void DoUpdateBattleData();

  UFUNCTION(BlueprintCallable, Category = Combat)
  FBattleData& GetBattleDataByUID(int32 UID);
  
  UFUNCTION(BlueprintCallable, Category = Combat)
  TArray<AMapBasicEntity*> GetNextTurnEntities(int32 n);
};
