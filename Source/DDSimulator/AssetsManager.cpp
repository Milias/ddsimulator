// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
#include "AssetsManager.h"

AAssetsManager::AAssetsManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AAssetsManager::BeginPlay()
{
	Super::BeginPlay();
}

void AAssetsManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}
