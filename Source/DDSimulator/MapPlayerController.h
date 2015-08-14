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

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Selection)
  TArray<AMapBasicEntity*> SelectedEntity;

  AMapPlayerController(const FObjectInitializer & PCIP);
  
  virtual void SetupInputComponent() override;

  UFUNCTION(BlueprintCallable, Category = CameraInputs)
  void CameraMoveForward(float d);

  UFUNCTION(BlueprintCallable, Category = CameraInputs)
  void CameraMoveRight(float d);

  UFUNCTION(BlueprintCallable, Category = CameraInputs)
  void CameraZoomIn(float d);

  UFUNCTION(BlueprintCallable, Category = CameraInputs)
  void CameraStartDrag();

  UFUNCTION(BlueprintCallable, Category = CameraInputs)
  void CameraStopDrag();

  UFUNCTION(BlueprintCallable, Category = CameraInputs)
  void CameraDrag(float d);

  UFUNCTION(BlueprintCallable, Category = Selection)
  void SelectEntityByIndex(const FTileIndex& p);

  UFUNCTION(BlueprintCallable, Category = Selection)
  void TraceSelection();

  UFUNCTION(BlueprintCallable, Category = Selection)
  void SelectEntities(const TArray<AMapBasicEntity*>& Selection);
};
