// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
#include "MapPlayerPawn.h"

AMapPlayerPawn::AMapPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;
  bReplicates = true;
  bAlwaysRelevant = true;
  bFindCameraComponentWhenViewTarget = true;

  /* Root component. */
  SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComponent"));

  /* Camera */
  RootComponent->SetVisibility(true);
  SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
  SpringArm->AttachTo(RootComponent);
  SpringArm->TargetArmLength = 1500.0f;
  SpringArm->bEnableCameraLag = true;
  SpringArm->SetRelativeRotation(FRotator(-80, 0, 0));
  SpringArm->bDoCollisionTest = false;
  SpringArm->bAbsoluteRotation = true;
  PlayerCamera = CreateDefaultSubobject<UCameraComponent>("PlayerEye");
  PlayerCamera->AttachTo(SpringArm, USpringArmComponent::SocketName);
  PlayerCamera->bUsePawnControlRotation = false;

  /* Moving Phantom Mesh */
  MovingEntityMesh = CreateDefaultSubobject<UStaticMeshComponent>("PhantomMesh");
  MovingEntityMesh->AttachTo(RootComponent);
  MovingEntityMesh->SetVisibility(false);
  MovingEntityMesh->SetMobility(EComponentMobility::Movable);
  MovingEntityMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

  TranslucidMaterial = ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'/Game/Materials/M_Transparent.M_Transparent'")).Object;
}

void AMapPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
  RootComponent->SetWorldLocation(FVector::ZeroVector);
  MovingEntityMesh->SetMaterial(0, UMaterialInstanceDynamic::Create(TranslucidMaterial, this));
}

void AMapPlayerPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void AMapPlayerPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
}
