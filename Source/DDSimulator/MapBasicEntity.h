// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "MapBasicEntity.generated.h"

class AMapTile;
class AMapPlayerController;

USTRUCT()
struct FTileIndex
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  int32 X;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  int32 Y;

  float f_score;
  float g_score;
  FTileIndex* parent;

  FTileIndex operator+(const FTileIndex& a) const { return FTileIndex(X + a.X, Y + a.Y); }
  FTileIndex operator-(const FTileIndex& a) const { return FTileIndex(X - a.X, Y - a.Y); }
  FTileIndex& operator=(const FTileIndex& a) { X = a.X; Y = a.Y; f_score = a.f_score; g_score = a.g_score; parent = a.parent; return *this; }
  FTileIndex& operator=(const TArray<int32>& a) { X = a[0]; Y = a[1]; return *this; }
  bool operator<(const FTileIndex& a) { return g_score < a.g_score; }
  bool operator>(const FTileIndex& a) { return g_score > a.g_score; }
  bool operator==(const FTileIndex& a) const { return X == a.X && Y == a.Y; }

  int32 MaxDist(const FTileIndex& a) const { return std::max<int32>(abs(X - a.X), abs(Y - a.Y)); }
  int32 AbsDist(const FTileIndex& a) const { return abs(X - a.X) + abs(Y - a.Y); }
  float SqrDist(const FTileIndex& a) const { return (X - a.X)*(X - a.X) + (Y - a.Y)*(Y - a.Y); }
  void ResetScores() { f_score = 0; g_score = 0; parent = NULL; }
  void operator()(int32 i = 0, int32 j = 0) { X = i; Y = j; f_score = 0; g_score = 0; parent = NULL; }

  FTileIndex(int32 x = 0, int32 y = 0) : X(x), Y(y), f_score(0), g_score(0), parent(NULL) {}
  FTileIndex(const FTileIndex& a) { X = a.X; Y = a.Y; f_score = a.f_score; g_score = a.g_score; parent = a.parent; }
};

USTRUCT()
struct FEntityActions
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  bool HasActions;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  TArray<int32> Standard;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  TArray<int32> Minor;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  TArray<int32> Movement;

  void Reset() { Standard[0] = Standard[1]; Minor[0] = Minor[1]; Movement[0] = Movement[1]; }
  void Zeroed() { Standard[0] = 0; Minor[0] = 0; Movement[0] = 0; }

  bool UseStandardAction() { if (Standard[0] > 0 && HasActions) { Standard[0]--; return true; } else { return false; } }
  bool UseMinorAction() { if (Minor[0] > 0 && HasActions) { Minor[0]--; return true; } else { return false; } }
  bool UseMovementAction() { if (Movement[0] > 0 && HasActions) { Movement[0]--; return true; } else { return false; } }

  FEntityActions(int32 std = 1, int32 mnr = 1, int32 mv = 1) : HasActions(true)
  {
    Standard.SetNum(2); Minor.SetNum(2); Movement.SetNum(2);
    Standard[0] = Standard[1] = std;
    Minor[0] = Minor[1] = mnr;
    Movement[0] = Movement[1] = mv;
  }
};

USTRUCT()
struct FEntityCombat
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  bool CanEnterCombat;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  bool HasTurn;
  
  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  TArray<int32> Initiative;

  UPROPERTY(BlueprintReadWrite, EditAnywhere)
  TArray<int32> Health;

  FEntityCombat() : CanEnterCombat(true), HasTurn(false)
  {
    Initiative.SetNumZeroed(2);
    Health.SetNumZeroed(2);
    Health[0] = Health[1] = 100;
  }
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
  UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = Movement)
  TArray<FTileIndex> MovingTiles;

  UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = Movement)
  TArray<FTileIndex> ProposedMovingTiles;

  UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = Entity)
  FVector CenterLocation;

  UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = Movement)
  FVector NextCenterLocation;

  UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
  float MovingTime;

  UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Movement)
  float MaxMovingTime;

  UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Movement)
  bool StopMovement;

  UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Movement)
  bool EntityMoving;

  UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Movement)
  int32 EntitySize;

  UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Combat)
  int32 InvolvedBattles;

  UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Combat)
  FEntityCombat CombatAttributes;

  UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = Combat)
  FEntityActions Actions;

  UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = Combat)
  FString GameOwner;

  /* PathFinding variables and functions */
  TArray<FTileIndex*> open;
  TArray<FTileIndex*> closed;

  AMapBasicEntity();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

  /****** Map ******/
  
  UFUNCTION(BlueprintCallable, Category = Map)
  void AssignTiles(const TArray<AMapTile*>& tiles);

  UFUNCTION(Server, WithValidation, Reliable)
  void ServerAssignTiles(const TArray<AMapTile*>& tiles);

  void DoAssignTiles(const TArray<AMapTile*>& tiles);

  /******* Movement *******/

  FTileIndex* GetLowest(TArray<FTileIndex*>& arr);

  UFUNCTION(BlueprintCallable, Category = Movement)
  void FindPathTo(const TArray<FTileIndex>& Tiles);

  UFUNCTION(Server, WithValidation, Reliable)
  void ServerFindPathTo(const TArray<FTileIndex>& Tiles);

  void DoFindPathTo(const TArray<FTileIndex>& Tiles);
  void DoFindPathTo(const FTileIndex& Tile);

  UFUNCTION(BlueprintCallable, Category = Movement)
  void MoveTo(const TArray<FTileIndex>& steps);

  UFUNCTION(Server, WithValidation, Reliable)
  void ServerMoveTo(const TArray<FTileIndex>& steps);

  void DoMoveTo(const TArray<FTileIndex>& steps);

  UFUNCTION(BlueprintCallable, Category = Movement)
  void ReconstructPath(const FTileIndex& dest, const FTileIndex& origin);

  UFUNCTION(Server, WithValidation, Reliable)
  void ServerReconstructPath(const FTileIndex& dest, const FTileIndex& origin);

  void DoReconstructPath(const FTileIndex& dest, const FTileIndex& origin);

  UFUNCTION(BlueprintCallable, Category = Movement)
  void FollowProposedPath();

  UFUNCTION(Server, WithValidation, Reliable)
  void ServerFollowProposedPath();

  void DoFollowProposedPath();

  /***** Combat *****/

  UFUNCTION(BlueprintCallable, Category = Combat)
  void EnterTurn();

  UFUNCTION(Server, WithValidation, Reliable)
  void ServerEnterTurn();

  void DoEnterTurn();
  
  UFUNCTION(BlueprintCallable, Category = Combat)
  void ExitTurn();

  UFUNCTION(Server, WithValidation, Reliable)
  void ServerExitTurn();

  void DoExitTurn();

  UFUNCTION(BlueprintCallable, Category = Combat)
  void EnterCombat();

  UFUNCTION(Server, WithValidation, Reliable)
  void ServerEnterCombat();

  void DoEnterCombat();
  
  UFUNCTION(BlueprintCallable, Category = Combat)
  void ExitCombat();

  UFUNCTION(Server, WithValidation, Reliable)
  void ServerExitCombat();

  void DoExitCombat();

  /****** Entity ******/

  UFUNCTION(BlueprintCallable, Category = Entity)
  void Register(const FString& MapPlayerOwner, const FTileIndex& pos, int32 size);

  UFUNCTION(Server, WithValidation, Reliable)
  void ServerRegister(const FString& MapPlayerOwner, const FTileIndex& pos, int32 size);

  void DoRegister(const FString& MapPlayerOwner, const FTileIndex& pos, int32 size);

  UFUNCTION(BlueprintCallable, Category = Entity)
  void UnRegister();

  UFUNCTION(Server, WithValidation, Reliable)
  void ServerUnRegister();

  void DoUnRegister();
};
