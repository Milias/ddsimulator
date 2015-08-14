// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "MapBasicEntity.generated.h"

class AMapTile;

USTRUCT()
struct FTileIndex
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  int32 X;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  int32 Y;

  FTileIndex operator+(const FTileIndex& a) const { return FTileIndex(X + a.X, Y + a.Y); }
  FTileIndex operator-(const FTileIndex& a) const { return FTileIndex(X - a.X, Y - a.Y); }
  FTileIndex& operator=(const FTileIndex& a) { X = a.X; Y = a.Y; return *this; }
  FTileIndex& operator=(const TArray<int32>& a) { X = a[0]; Y = a[1]; return *this; }
  bool operator==(const FTileIndex& a) const { return X == a.X && Y == a.Y; }

  int32 MaxDist(const FTileIndex& a) const { return std::max<int32>(abs(X - a.X), abs(Y - a.Y)); }
  int32 AbsDist(const FTileIndex& a) const { return abs(X - a.X) + abs(Y - a.Y); }
  void operator()(int32 i = 0, int32 j = 0) { X = i; Y = j; }

  FTileIndex(int32 x = 0, int32 y = 0) : X(x), Y(y) {}
};

UCLASS()
class DDSIMULATOR_API AMapBasicEntity : public APawn
{
	GENERATED_BODY()

public:
  UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Entity)
  int32 uid;

  UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Entity)
  UStaticMeshComponent * Mesh;

  UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = Entity)
  TArray<AMapTile*> AssignedTiles;

  /*
    Allocates a number of steps to be followed by this entity.
  */
  UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = Entity)
  TArray<FTileIndex> MovingTiles;

  UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = Entity)
  FVector CenterLocation;

  UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = Entity)
  FVector NextCenterLocation;

  UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Entity)
  float MovingTime;

  UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Entity)
  float MaxMovingTime;

  UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Entity)
  bool StopMovement;

  AMapBasicEntity();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
  
  UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = Entity)
  void AssignTiles(const TArray<AMapTile*>& tiles);

  UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = Entity)
  void MoveTo(const TArray<FTileIndex>& steps);

  UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = Entity)
  void UnRegister();
};
