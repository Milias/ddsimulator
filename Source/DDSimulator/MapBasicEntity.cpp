// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
#include "MapPlayerController.h"
#include "MapState.h"
#include "MapBasicEntity.h"

AMapBasicEntity::AMapBasicEntity() : uid(0), CenterLocation(FVector::ZeroVector), NextCenterLocation(FVector::ZeroVector), MovingTime(0), MaxMovingTime(0.25f), StopMovement(false), EntityMoving(false), Actions(FEntityActions())
{
  PrimaryActorTick.bCanEverTick = true;
  bReplicates = true;
  bAlwaysRelevant = true;
  bFindCameraComponentWhenViewTarget = true;
  SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComponent"));
  Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
  Mesh->AttachTo(RootComponent);
  Mesh->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Meshes/Basic/Sphere.Sphere'")).Object);
  Mesh->SetCollisionResponseToChannel(COLLISION_MAP_TILE, ECollisionResponse::ECR_Ignore);
  Mesh->SetCollisionResponseToChannel(COLLISION_MAP_BASIC_ENTITY, ECollisionResponse::ECR_Block);
}

void AMapBasicEntity::BeginPlay()
{
	Super::BeginPlay();
}

void AMapBasicEntity::Tick( float DeltaTime )
{
  Super::Tick(DeltaTime);

  if (MovingTiles.Num() && !StopMovement) {
    if (GetWorld()->GameState == NULL) { return; }
    if (Cast<AMapState>(GetWorld()->GameState)->Map == NULL) { return; }
    EntityMoving = true;
    AMapBase * map = Cast<AMapState>(GetWorld()->GameState)->Map;
    MovingTime += DeltaTime;

    if (MovingTime > MaxMovingTime) {
      MovingTime = 0;
      CenterLocation = NextCenterLocation;
      for (int32 i = 0; i != AssignedTiles.Num(); i++) {
        AssignedTiles[i]->AssignEntity(this);
        AssignedTiles[i] = map->GetTileFromIndex(AssignedTiles[i]->Index + MovingTiles[0]);
        AssignedTiles[i]->AssignEntity(this);
      }
      MovingTiles.RemoveAt(0);
      if (MovingTiles.Num() == 0) { EntityMoving = false; }
    }

    if (CenterLocation == NextCenterLocation && MovingTiles.Num() > 0 ) {
      NextCenterLocation = FVector::ZeroVector;
      for (int32 i = 0; i != AssignedTiles.Num(); i++) {
        NextCenterLocation += map->GetTileFromIndex(AssignedTiles[i]->Index + MovingTiles[0])->EntityPosition();
      } NextCenterLocation /= AssignedTiles.Num();
    }

    SetActorLocation(FMath::Lerp<FVector, float>(CenterLocation, NextCenterLocation, MovingTime / MaxMovingTime));
  }
}

void AMapBasicEntity::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
}

void AMapBasicEntity::AssignTiles(const TArray<AMapTile*>& tiles)
{
  if (Role < ROLE_Authority) {
    ServerAssignTiles(tiles);
  } else {
    DoAssignTiles(tiles);
  }
}

void AMapBasicEntity::ServerAssignTiles_Implementation(const TArray<AMapTile*>& tiles)
{
  DoAssignTiles(tiles);
}

bool AMapBasicEntity::ServerAssignTiles_Validate(const TArray<AMapTile*>& tiles)
{
  return true;
}

void AMapBasicEntity::DoAssignTiles(const TArray<AMapTile*>& tiles)
{
  CenterLocation = FVector::ZeroVector;
  for (auto it = tiles.CreateConstIterator(); it; ++it) {
    (*it)->AssignEntity(this);
    CenterLocation += (*it)->EntityPosition();
  }
  CenterLocation /= tiles.Num();
  NextCenterLocation = CenterLocation;
  AssignedTiles.Append(tiles);
  SetActorLocation(CenterLocation);
}

void AMapBasicEntity::MoveTo(const TArray<FTileIndex>& steps)
{
  if (Role < ROLE_Authority) {
    ServerMoveTo(steps);
  } else {
    DoMoveTo(steps);
  }
}

void AMapBasicEntity::ServerMoveTo_Implementation(const TArray<FTileIndex>& steps)
{
  DoMoveTo(steps);
}

bool AMapBasicEntity::ServerMoveTo_Validate(const TArray<FTileIndex>& steps)
{
  return true;
}

void AMapBasicEntity::DoMoveTo(const TArray<FTileIndex>& steps)
{
  MovingTiles.Append(steps);
}

void AMapBasicEntity::FindPathTo(const TArray<FTileIndex>& Tiles)
{
  if (Role < ROLE_Authority) {
    ServerFindPathTo(Tiles);
  } else {
    DoFindPathTo(Tiles);
  }
}

void AMapBasicEntity::ServerFindPathTo_Implementation(const TArray<FTileIndex>& Tiles)
{
  DoFindPathTo(Tiles);
}

bool AMapBasicEntity::ServerFindPathTo_Validate(const TArray<FTileIndex>& Tiles)
{
  return true;
}

void AMapBasicEntity::DoFindPathTo(const TArray<FTileIndex>& Tiles)
{
  for (int32 i = 0; i != Tiles.Num(); i++) { DoFindPathTo(Tiles[i]); }
}

FTileIndex* AMapBasicEntity::GetLowest(TArray<FTileIndex*>& arr)
{
  FTileIndex* R = arr[0]; float g = arr[0]->g_score;
  for (int32 i = 1; i != arr.Num(); i++) {
    if (arr[i]->g_score < g) { R = arr[i]; }
  } return R;
}

void AMapBasicEntity::ReconstructPath(const FTileIndex& dest, const FTileIndex& origin)
{
  if (Role < ROLE_Authority) {
    ServerReconstructPath(dest,origin);
  } else {
    DoReconstructPath(dest,origin);
  }
}

void AMapBasicEntity::ServerReconstructPath_Implementation(const FTileIndex& dest, const FTileIndex& origin)
{
  DoReconstructPath(dest,origin);
}

bool AMapBasicEntity::ServerReconstructPath_Validate(const FTileIndex& dest, const FTileIndex& origin)
{
  return true;
}

void AMapBasicEntity::DoReconstructPath(const FTileIndex& dest, const FTileIndex& origin)
{
  ProposedMovingTiles.Empty();
  AMapBase * map = Cast<AMapState>(GetWorld()->GameState)->Map;
  const FTileIndex* current = &origin;

  do {
    ProposedMovingTiles.Insert(*current - *current->parent,0);
    current = current->parent;
  } while (current->parent);
}

void AMapBasicEntity::DoFindPathTo(const FTileIndex& Tile)
{
  if (AssignedTiles[0]->Index == Tile) { return; }

  float g;
  AMapBase * map = Cast<AMapState>(GetWorld()->GameState)->Map;
  AMapTile * tile; FTileIndex * it; bool isItInOpen = false;

  for (int32 i = 0; i != map->MapTiles.Num(); i++) { map->MapTiles[i]->Index.ResetScores(); }
  open.Empty(); closed.Empty();

  FTileIndex * start = &AssignedTiles[0]->Index;
  open.Add(start);
  open[0]->f_score = open[0]->SqrDist(Tile);

  FTileIndex* current = open[0]; 

  while (open.Num()) {
    if (open.Num() == 1) { current = open[0]; } else { current = GetLowest(open); }
    if (*current == Tile) { ReconstructPath(*start,*current); break; }
    
    open.Remove(current);
    closed.Add(current);

    tile = map->GetTileFromIndex(*current);
    for (int32 i = 0; i != tile->OpenTiles.Num(); i++) {
      if (tile->OpenTiles[i]->Clearance < EntitySize || !tile->OpenTiles[i]->IsTransitable()) { continue; }
      it = &tile->OpenTiles[i]->Index;
      if (closed.FindByPredicate([&](FTileIndex* a) { return *a == *it; }) != nullptr) { continue; }
      g = current->g_score + 1;
      isItInOpen = open.FindByPredicate([&](FTileIndex* a) { return *a == *it; }) == nullptr;
      if (isItInOpen || (g < it->g_score)) {
        it->parent = current;
        it->g_score = g;
        it->f_score = g + it->SqrDist(Tile);
        if (isItInOpen) { open.Add(it); }
      }
    }
  }
}

void AMapBasicEntity::FollowProposedPath()
{
  if (Role < ROLE_Authority) {
    ServerFollowProposedPath();
  } else {
    DoFollowProposedPath();
  }
}

void AMapBasicEntity::ServerFollowProposedPath_Implementation()
{
  DoFollowProposedPath();
}

bool AMapBasicEntity::ServerFollowProposedPath_Validate()
{
  return true;
}

void AMapBasicEntity::DoFollowProposedPath()
{
  if (ProposedMovingTiles.Num() == 0) { return; }
  MoveTo(ProposedMovingTiles);
  ProposedMovingTiles.Empty();
}

void AMapBasicEntity::Register(const FString& owner, const FTileIndex& pos, int32 size)
{
  if (Role < ROLE_Authority) {
    ServerRegister(owner,pos,size);
  } else {
    DoRegister(owner,pos,size);
  }
}

void AMapBasicEntity::ServerRegister_Implementation(const FString& owner, const FTileIndex& pos, int32 size)
{
  DoRegister(owner,pos,size);
}

bool AMapBasicEntity::ServerRegister_Validate(const FString& owner, const FTileIndex& pos, int32 size)
{
  return true;
}

void AMapBasicEntity::DoRegister(const FString& owner, const FTileIndex& pos, int32 size)
{
  TArray<AMapTile*> tiles;

  for (int32 i = 0; i < size; i++) {
    for (int32 j = 0; j < size; j++) {
      tiles.Add(Cast<AMapState>(GetWorld()->GameState)->Map->GetTileFromIndex(pos + FTileIndex(i, j)));
    }
  }

  AssignTiles(tiles);
  GameOwner = owner;
  Cast<AMapState>(GetWorld()->GameState)->MapEntities.Add(this);
  EntitySize = size;
}

void AMapBasicEntity::UnRegister()
{
  if (Role < ROLE_Authority) {
    ServerUnRegister();
  } else {
    DoUnRegister();
  }
}

void AMapBasicEntity::ServerUnRegister_Implementation()
{
  DoUnRegister();
}

bool AMapBasicEntity::ServerUnRegister_Validate()
{
  return true;
}

void AMapBasicEntity::DoUnRegister()
{
  for (int32 i = 0; i != AssignedTiles.Num(); i++) {
    AssignedTiles[i]->AssignEntity(this);
  }
  Cast<AMapState>(GetWorld()->GameState)->MapEntities.Remove(this);
}

void AMapBasicEntity::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
  DOREPLIFETIME(AMapBasicEntity, uid);
  DOREPLIFETIME(AMapBasicEntity, Mesh);
  DOREPLIFETIME(AMapBasicEntity, AssignedTiles);
  DOREPLIFETIME(AMapBasicEntity, MovingTiles);
  DOREPLIFETIME(AMapBasicEntity, ProposedMovingTiles);
  DOREPLIFETIME(AMapBasicEntity, MovingTime);
  DOREPLIFETIME(AMapBasicEntity, MaxMovingTime);
  DOREPLIFETIME(AMapBasicEntity, CenterLocation);
  DOREPLIFETIME(AMapBasicEntity, NextCenterLocation);
  DOREPLIFETIME(AMapBasicEntity, StopMovement);
  DOREPLIFETIME(AMapBasicEntity, EntitySize);
  DOREPLIFETIME(AMapBasicEntity, Actions);
  DOREPLIFETIME(AMapBasicEntity, GameOwner);
}
