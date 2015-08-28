// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Power.generated.h"

class AMapBasicEntity;

USTRUCT()
struct FPowerData
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 UID;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Name;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 Level;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Class;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FText FlavorText;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Kind; //"attack", "utility"

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Usage; //"at-will", "encounter", "daily", "recharge#"

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString ActionType; //"standard", "minor", "movement", "free", "interrupt", "reaction", "trigger"

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FString> Keywords;

  /*
    1     2     3     4
    ===================
   -1     -     -     -   Personal
    0     0     -     -   Melee weapon
    0    -1     -     -   Melee touch
    0     #     -     -   Melee #
    1     0     -     -   Ranged weapon
    1    -1     -     -   Ranged sight
    1     #     -     -   Ranged #
    2     0     #     -   Close burst # (All directions)
    2     1     #     -   Close blast #
    3     0    #1    #2   Area burst #1 within #2
    3     1    #1    #2   Area wall #1 within #2
  */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<int32> Range;

  /*
  (#C, #E, #A, #O) - Number of creatures, enemies, allies and objects/unoccupied space.
  -1  ..........  Every possible target in range.
   0  ..........  Nothing.
  >0  ..........  Up to # units.
  <0  ..........  Exactly # units (except -1).
  */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<int32> Target;

  FPowerData& operator=(const FPowerData& a) {
    UID = a.UID;
    Name = a.Name;
    Level = a.Level;
    Class = a.Class;
    FlavorText = a.FlavorText;
    Kind = a.Kind;
    Usage = a.Usage;
    ActionType = a.ActionType;
    Keywords = a.Keywords;
    Range = a.Range;
    Target = a.Target;
    return *this;
  }

  FPowerData(int32 UID = 0, FString Name = "", int32 Level = 1, FString Class = "", FText FlavorText = FText::GetEmpty(), FString Kind = "", FString Usage = "", FString ActionType = "") : 
    UID(UID),
    Name(Name),
    Level(Level),
    Class(Class),
    FlavorText(FlavorText),
    Kind(Kind),
    Usage(Usage),
    ActionType(ActionType)
  {
    Keywords.SetNumZeroed(0);
    Range.SetNumZeroed(4);
    Target.SetNumZeroed(4);
  }
};

UCLASS()
class DDSIMULATOR_API APower : public AActor
{
	GENERATED_BODY()
	
public:
  UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Power)
  AMapBasicEntity * OwnerEntity;

  UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Power)
  FPowerData Data;

  UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Power)
  bool IsInitialized;

  APower();

  UFUNCTION(BlueprintCallable, Category = Power)
  void SetOwnerEntity(AMapBasicEntity * Entity);
  
  UFUNCTION(Server, WithValidation, Reliable)
  void ServerSetOwnerEntity(AMapBasicEntity * Entity);

  void DoSetOwnerEntity(AMapBasicEntity * Entity);

  UFUNCTION(BlueprintCallable, Category = Power)
  void Initialize(int32 UID);
  
  UFUNCTION(Server, WithValidation, Reliable)
  void ServerInitialize(int32 UID);

  void DoInitialize(int32 UID);

  UFUNCTION(BlueprintCallable, Category = Power)
  void PowerAction(const TArray<AMapBasicEntity*>& Targets);

  UFUNCTION(Server, WithValidation, Reliable)
  void ServerPowerAction(const TArray<AMapBasicEntity*>& Targets);

  UFUNCTION(BlueprintNativeEvent)
  void DoPowerAction(const TArray<AMapBasicEntity*>& Targets);
};
