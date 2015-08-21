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
  InputComponent->BindAxis("MouseMoving", this, &AMapPlayerController::MouseMoving);
  InputComponent->BindAction("SelectEntity", IE_Pressed, this, &AMapPlayerController::TraceSelection);
  InputComponent->BindAction("RightClick", EInputEvent::IE_Pressed, this, &AMapPlayerController::RightClickPressed);
  InputComponent->BindAction("RightClick", EInputEvent::IE_Released, this, &AMapPlayerController::RightClickReleased);

  InputComponent->BindAction("Spawn", EInputEvent::IE_Pressed, this, &AMapPlayerController::Spawn);
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
    Cast<AMapPlayerPawn>(GetPawn())->SpringArm->TargetArmLength = FMath::Clamp<float>(Cast<AMapPlayerPawn>(GetPawn())->SpringArm->TargetArmLength, 250, 2500);
  }
}

void AMapPlayerController::RightClickPressed()
{
  PressingRightClick = true;

  /** Camera draggin' **/
  float x; float y;
  GetMousePosition(x, y);
  CameraFirstRightClick.X = x;
  CameraFirstRightClick.Y = y;
  CameraInitialPosition = GetPawn()->GetActorLocation();
}

void AMapPlayerController::RightClickReleased()
{
  /** Movin' entity **/
  if (Cast<AMapPlayerState>(PlayerState) != NULL && Cast<AMapHUD>(MyHUD)->MovingEntity) {
    TArray<AMapBasicEntity*>& SelectedEntity = Cast<AMapPlayerState>(PlayerState)->SelectedEntity;
    if (SelectedEntity.Num() == 1 && !CameraDragging) {
      AMapTile * tile = GetTileUnderCursor();
      if (tile) { MoveSelectionToTile(SelectedEntity[0], tile->Index); }
    }
  }

  PressingRightClick = false;
  CameraDragging = false;
  bShowMouseCursor = true;
}

void AMapPlayerController::MouseMoving(float d)
{
  if (Cast<AMapPlayerPawn>(GetPawn()) == NULL || Cast<AMapPlayerState>(PlayerState) == NULL) { return; }
  float x; float y; FVector2D CurrentPosition; AMapPlayerPawn * pawn = NULL; TArray<AMapBasicEntity*>* SelectedEntity = NULL;
  if (PressingRightClick || Cast<AMapHUD>(MyHUD)->MovingEntity) {
    GetMousePosition(x, y);
    CurrentPosition = FVector2D(x, y);
    pawn = Cast<AMapPlayerPawn>(GetPawn());
    SelectedEntity = &Cast<AMapPlayerState>(PlayerState)->SelectedEntity;
  }
  if (PressingRightClick) {
    if (FVector2D::Distance(CurrentPosition, CameraFirstRightClick) > CameraDragDeadZone) {
      CameraDragging = true;
      bShowMouseCursor = false;
      FVector2D t = pawn->SpringArm->TargetArmLength/500 * (CurrentPosition - CameraFirstRightClick);
      pawn->SetActorLocation(FVector(t.Y, -t.X, 0) + CameraInitialPosition);
      pawn->MovingEntityMesh->SetVisibility(false);
    } else {
      CameraDragging = false;
    }
  }
  if (Cast<AMapHUD>(MyHUD)->MovingEntity && !(*SelectedEntity)[0]->EntityMoving) {
    AMapTile * tile = GetTileUnderCursor();
    if (tile) {
      if (tile->Clearance >= (*SelectedEntity)[0]->EntitySize) {
        pawn->MovingEntityMesh->SetStaticMesh((*SelectedEntity)[0]->Mesh->StaticMesh);
        pawn->MovingEntityMesh->SetWorldLocation(tile->EntityPosition() + (*SelectedEntity)[0]->CenterLocation - (*SelectedEntity)[0]->AssignedTiles[0]->EntityPosition());
        pawn->MovingEntityMesh->SetVisibility(true);
      }
    }
  } else { Cast<AMapPlayerPawn>(GetPawn())->MovingEntityMesh->SetVisibility(false); }
}

TArray<AMapBasicEntity*> AMapPlayerController::GetEntitiesInTrace()
{
  TArray<AMapBasicEntity*> R;
  FHitResult TraceResult(ForceInit);
  GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Pawn), false, TraceResult);
  if (TraceResult.GetActor() != nullptr)
  {
    if (Cast<AMapBasicEntity>(TraceResult.GetActor()) != NULL) {
      R.Add(Cast<AMapBasicEntity>(TraceResult.GetActor()));
    }
    if (Cast<AMapTile>(TraceResult.GetActor())) {
      R.Append(Cast<AMapTile>(TraceResult.GetActor())->AssignedEntity);
    }
  }
  return R;
}

void AMapPlayerController::TraceSelection()
{
  SelectEntities(GetEntitiesInTrace());
}

void AMapPlayerController::SelectEntities(const TArray<AMapBasicEntity*>& Selection)
{
  if (Cast<AMapPlayerState>(PlayerState) == NULL) { return; }
  Cast<AMapPlayerState>(PlayerState)->SelectedEntity.Empty();
  Cast<AMapPlayerState>(PlayerState)->SelectedEntity.Append(Selection);
  if (Cast<AMapHUD>(MyHUD) != NULL) {
    Cast<AMapHUD>(MyHUD)->UpdateSelection(Cast<AMapPlayerState>(PlayerState)->SelectedEntity);
  }
}

AMapTile * AMapPlayerController::GetTileUnderCursor()
{
  AMapTile * tile = NULL;
  FHitResult TraceResult(ForceInit);
  GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(COLLISION_MAP_TILE), false, TraceResult);
  if (TraceResult.GetActor() != NULL) {
    if (Cast<AMapTile>(TraceResult.GetActor())) {
      tile = Cast<AMapTile>(TraceResult.GetActor());
    }
    //if (Cast<AMapBasicEntity>(TraceResult.GetActor())) {
      //tile = Cast<AMapBasicEntity>(TraceResult.GetActor())->AssignedTiles[0];
    //}
  }
  return tile;
}

void AMapPlayerController::MoveSelectionToTile(AMapBasicEntity* Entity, const FTileIndex& Tile)
{
  if (Role < ROLE_Authority) {
    ServerMoveSelectionToTile(Entity,Tile);
  } else {
    if (Entity->GameOwner != GetName()) { return; }
    DoMoveSelectionToTile(Entity,Tile);
  }
}

void AMapPlayerController::ServerMoveSelectionToTile_Implementation(AMapBasicEntity* Entity, const FTileIndex& Tile)
{
  if (Entity->GameOwner != GetName()) { return; }
  DoMoveSelectionToTile(Entity,Tile);
}

bool AMapPlayerController::ServerMoveSelectionToTile_Validate(AMapBasicEntity* Entity, const FTileIndex& Tile)
{
  return true;
}

void AMapPlayerController::DoMoveSelectionToTile(AMapBasicEntity* Entity, const FTileIndex& Tile)
{
  if (Entity->MovingTiles.Num() == 0) {
    TArray<FTileIndex> t; t.Add(Tile);
    Entity->FindPathTo(t);
    Entity->FollowProposedPath();
  }
}

void AMapPlayerController::Spawn_Implementation()
{
  GetWorld()->SpawnActor<AMapBasicEntity>(AMapBasicEntity::StaticClass())->Register(GetName(), FTileIndex(), 1);
  GetWorld()->SpawnActor<AMapBasicEntity>(AMapBasicEntity::StaticClass())->Register(GetName(), FTileIndex(), 2);
  GetWorld()->SpawnActor<AMapBasicEntity>(AMapBasicEntity::StaticClass())->Register(GetName(), FTileIndex(), 3);
}

bool AMapPlayerController::Spawn_Validate()
{
  return true;
}
