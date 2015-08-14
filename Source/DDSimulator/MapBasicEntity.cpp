// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
#include "MapState.h"
#include "MapBasicEntity.h"

AMapBasicEntity::AMapBasicEntity() : uid(0), CenterLocation(FVector::ZeroVector), NextCenterLocation(FVector::ZeroVector), MovingTime(0), MaxMovingTime(1.5f), StopMovement(false)
{
  PrimaryActorTick.bCanEverTick = true;
  bReplicates = true;
  bAlwaysRelevant = true;
  bFindCameraComponentWhenViewTarget = true;
  SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComponent"));
  //RootComponent->SetNetAddressable();
  Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
  Mesh->AttachTo(RootComponent);
  Mesh->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Meshes/Basic/Sphere.Sphere'")).Object);
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

void AMapBasicEntity::AssignTiles_Implementation(const TArray<AMapTile*>& tiles)
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

bool AMapBasicEntity::AssignTiles_Validate(const TArray<AMapTile*>& tiles)
{
  return true;
}

void AMapBasicEntity::MoveTo_Implementation(const TArray<FTileIndex>& steps)
{
  MovingTiles.Append(steps);
}

bool AMapBasicEntity::MoveTo_Validate(const TArray<FTileIndex>& steps)
{
  return true;
}

void AMapBasicEntity::UnRegister_Implementation()
{
  for (int32 i = 0; i != AssignedTiles.Num(); i++) {
    AssignedTiles[i]->AssignEntity(this);
  }
}

bool AMapBasicEntity::UnRegister_Validate()
{
  return true;
}

void AMapBasicEntity::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
  DOREPLIFETIME(AMapBasicEntity, uid);
  DOREPLIFETIME(AMapBasicEntity, Mesh);
  DOREPLIFETIME(AMapBasicEntity, AssignedTiles);
  DOREPLIFETIME(AMapBasicEntity, MovingTiles);
  DOREPLIFETIME(AMapBasicEntity, MovingTime);
  DOREPLIFETIME(AMapBasicEntity, MaxMovingTime);
  DOREPLIFETIME(AMapBasicEntity, CenterLocation);
  DOREPLIFETIME(AMapBasicEntity, NextCenterLocation);
  DOREPLIFETIME(AMapBasicEntity, StopMovement);
}
