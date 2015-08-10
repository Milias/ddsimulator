// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "MapBasicEntity.generated.h"

class UMapTile;

UCLASS()
class DDSIMULATOR_API AMapBasicEntity : public APawn
{
	GENERATED_BODY()

public:
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Entity)
  int32 uid;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Entity)
  UStaticMeshComponent * Mesh;

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Entity)
  TArray<UMapTile*> AssignedTiles;

  UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Entity)
  TArray<UMapTile*> MovingTiles;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Entity)
  float MovingTime;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Entity)
  float MaxMovingTime;

  AMapBasicEntity();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
  
  UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = Entity)
  void AssignTiles(const TArray<UMapTile*>& tiles);

  UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = Entity)
  void MoveTo(const TArray<int32>& tiles);
};
