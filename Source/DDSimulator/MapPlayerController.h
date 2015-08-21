// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "MapPlayerPawn.h"
#include "MapTile.h"
#include "MapPlayerController.generated.h"

class AMapBasicEntity;
struct FTileIndex;

UCLASS()
class DDSIMULATOR_API AMapPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraInputs)
  float CameraMoveDistance;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraInputs)
  float CameraZoomDistance;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraInputs)
  float CameraDragDeadZone;

  FVector CameraInitialPosition;
  FVector2D CameraFirstRightClick;
  bool PressingRightClick;
  bool CameraDragging;

  AMapPlayerController(const FObjectInitializer & PCIP);
  
  virtual void SetupInputComponent() override;

  UFUNCTION(BlueprintCallable, Category = CameraInputs)
  void CameraMoveForward(float d);

  UFUNCTION(BlueprintCallable, Category = CameraInputs)
  void CameraMoveRight(float d);

  UFUNCTION(BlueprintCallable, Category = CameraInputs)
  void CameraZoomIn(float d);

  UFUNCTION(BlueprintCallable, Category = CameraInputs)
  void RightClickPressed();

  UFUNCTION(BlueprintCallable, Category = CameraInputs)
  void RightClickReleased();

  UFUNCTION(BlueprintCallable, Category = CameraInputs)
  void MouseMoving(float d);

  UFUNCTION(BlueprintCallable, Category = Selection)
  TArray<AMapBasicEntity*> GetEntitiesInTrace();

  UFUNCTION(BlueprintCallable, Category = Selection)
  void TraceSelection();

  UFUNCTION(BlueprintCallable, Category = Selection)
  void SelectEntities(const TArray<AMapBasicEntity*>& Selection);

  UFUNCTION(BlueprintCallable, Category = Movement)
  AMapTile * GetTileUnderCursor();

  UFUNCTION(BlueprintCallable, Category = Movement)
  void MoveSelectionToTile(AMapBasicEntity* Entity, const FTileIndex& Tile);

  UFUNCTION(Server, WithValidation, Reliable)
  void ServerMoveSelectionToTile(AMapBasicEntity* Entity, const FTileIndex& Tile);

  void DoMoveSelectionToTile(AMapBasicEntity* Entity, const FTileIndex& Tile);

  UFUNCTION(Server, WithValidation, Reliable)
  void Spawn();
};
