// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
#include "MapPlayerController.h"
#include "MapBasicEntity.h"
#include "MapState.h"
#include "MapHUD.h"

AMapPlayerController::AMapPlayerController(const FObjectInitializer & PCIP) : Super(PCIP), CameraMoveDistance(20), CameraZoomDistance(50), CameraDragDeadZone(10)
{
  SetupInputComponent();
}

void AMapPlayerController::SetupInputComponent()
{
  bShowMouseCursor = true;

  InputComponent = NewObject<UInputComponent>();
  InputComponent->BindAxis("MoveForward", this, &AMapPlayerController::CameraMoveForward);
  InputComponent->BindAxis("MoveRight", this, &AMapPlayerController::CameraMoveRight);
  InputComponent->BindAxis("ZoomIn", this, &AMapPlayerController::CameraZoomIn);
  InputComponent->BindAxis("MouseMoving", this, &AMapPlayerController::CameraDrag);
  InputComponent->BindAction("SelectEntity", IE_Pressed, this, &AMapPlayerController::TraceSelection);
  InputComponent->BindAction("RightClick", EInputEvent::IE_Pressed, this, &AMapPlayerController::CameraStartDrag);
  InputComponent->BindAction("RightClick", EInputEvent::IE_Released, this, &AMapPlayerController::CameraStopDrag);
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
    Cast<AMapPlayerPawn>(GetPawn())->SpringArm->TargetArmLength = FMath::Clamp<float>(Cast<AMapPlayerPawn>(GetPawn())->SpringArm->TargetArmLength, 250, 1750);
  }
}

void AMapPlayerController::CameraStartDrag()
{
  float x; float y;
  GetMousePosition(x, y);
  CameraFirstRightClick.X = x;
  CameraFirstRightClick.Y = y;
  PressingRightClick = true;
  CameraInitialPosition = GetPawn()->GetActorLocation();
}

void AMapPlayerController::CameraStopDrag()
{
  PressingRightClick = false;
  bShowMouseCursor = true;
}

void AMapPlayerController::CameraDrag(float d)
{
  if (PressingRightClick) {
    float x; float y;
    GetMousePosition(x, y);
    FVector2D CurrentPosition(x, y);
    if (FVector2D::Distance(CurrentPosition, CameraFirstRightClick) > CameraDragDeadZone) {
      bShowMouseCursor = false;
      FVector2D t = Cast<AMapPlayerPawn>(GetPawn())->SpringArm->TargetArmLength/500 * (CurrentPosition - CameraFirstRightClick);
      GetPawn()->SetActorLocation(FVector(t.Y, -t.X, 0)+CameraInitialPosition);
    }
  }
}

void AMapPlayerController::SelectEntityByIndex(const FTileIndex& p)
{
  AMapTile * tile = Cast<AMapState>(GetWorld()->GameState)->Map->GetTileFromIndex(p);
  SelectedEntity.Append(tile->AssignedEntity);
}

void AMapPlayerController::TraceSelection()
{
  FHitResult TraceResult(ForceInit);
  GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Pawn), false, TraceResult);
  if (TraceResult.GetActor() != nullptr)
  {
    if (Cast<AMapBasicEntity>(TraceResult.GetActor()) != NULL) {
      TArray<AMapBasicEntity*> t; 
      t.Add(Cast<AMapBasicEntity>(TraceResult.GetActor()));
      SelectEntities(t);
    }
    if (Cast<AMapTile>(TraceResult.GetActor())) {
      SelectEntities(Cast<AMapTile>(TraceResult.GetActor())->AssignedEntity);
    }
  }
}

void AMapPlayerController::SelectEntities(const TArray<AMapBasicEntity*>& Selection)
{
  SelectedEntity.Empty();
  SelectedEntity.Append(Selection);
  if (Cast<AMapHUD>(MyHUD) != NULL) {
    Cast<AMapHUD>(MyHUD)->UpdateSelection(SelectedEntity);
  }
}
