// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
#include "MapPlayerController.h"
#include "MapPlayerState.h"
#include "MapState.h"

AMapPlayerController::AMapPlayerController(const FObjectInitializer & PCIP) : Super(PCIP), CameraMoveDistance(20), CameraZoomDistance(100)
{
  bShowMouseCursor = true;
  InputComponent = NewObject<UInputComponent>();
  InputComponent->BindAxis("MoveForward", this, &AMapPlayerController::CameraMoveForward);
  InputComponent->BindAxis("MoveRight", this, &AMapPlayerController::CameraMoveRight);
  InputComponent->BindAxis("ZoomIn", this, &AMapPlayerController::CameraZoomIn);
  InputComponent->BindAction("SelectEntity", IE_Pressed, this, &AMapPlayerController::TraceSelection);
}

void AMapPlayerController::Tick(float dt)
{
  Super::Tick(dt);
}

void AMapPlayerController::CameraMoveForward(float d)
{
  if (d != 0.0f)
  {
    GetPawn()->AddActorWorldOffset(FVector(CameraMoveDistance * d, 0, 0));
  }
}

void AMapPlayerController::CameraMoveRight(float d)
{
  if (d != 0.0f)
  {
    GetPawn()->AddActorWorldOffset(FVector(0, CameraMoveDistance * d, 0));
  }
}

void AMapPlayerController::CameraZoomIn(float d)
{
  if (d != 0.0f)
  {
    Cast<AMapPlayerPawn>(GetPawn())->SpringArm->TargetArmLength += d * CameraZoomDistance;
  }
}

void AMapPlayerController::SelectEntityByIndex(TArray<int32> p)
{
  UMapTile * tile = Cast<AMapState>(GetWorld()->GameState)->Map->GetTileFromIndex(p);
  Cast<AMapPlayerState>(PlayerState)->SelectedEntity.Append(tile->AssignedEntity);
}

void AMapPlayerController::TraceSelection()
{

}
