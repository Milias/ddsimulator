// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "MapPlayerPawn.h"
#include "MapPlayerController.generated.h"

UCLASS()
class DDSIMULATOR_API AMapPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraInputs)
  float CameraMoveDistance;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraInputs)
  float CameraZoomDistance;

  AMapPlayerController(const FObjectInitializer & PCIP);

  virtual void Tick(float dt) override;

  UFUNCTION(BlueprintCallable, Category = CameraInputs)
  void CameraMoveForward(float d);

  UFUNCTION(BlueprintCallable, Category = CameraInputs)
  void CameraMoveRight(float d);

  UFUNCTION(BlueprintCallable, Category = CameraInputs)
  void CameraZoomIn(float d);

  UFUNCTION(BlueprintCallable, Category = Selection)
  void SelectEntityByIndex(TArray<int32> p);

  UFUNCTION(BlueprintCallable, Category = Selection)
  void TraceSelection();
};
