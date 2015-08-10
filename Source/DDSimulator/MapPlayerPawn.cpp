// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
#include "MapPlayerPawn.h"

AMapPlayerPawn::AMapPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;
  bReplicates = true;
  bAlwaysRelevant = true;
  bFindCameraComponentWhenViewTarget = true;
  SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComponent"));
  RootComponent->SetVisibility(false);
  SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
  SpringArm->AttachTo(RootComponent);
  SpringArm->TargetArmLength = 1000.0f;
  SpringArm->bEnableCameraLag = true;
  SpringArm->SetRelativeRotation(FRotator(-80, 0, 0));
  SpringArm->bDoCollisionTest = false;
  SpringArm->bAbsoluteRotation = true;
  PlayerCamera = CreateDefaultSubobject<UCameraComponent>("PlayerEye");
  PlayerCamera->AttachTo(SpringArm, USpringArmComponent::SocketName);
  PlayerCamera->bUsePawnControlRotation = false;
}

void AMapPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
  RootComponent->SetWorldLocation(FVector::ZeroVector);
}

void AMapPlayerPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void AMapPlayerPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
}
