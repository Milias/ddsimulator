// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "AssetsManager.generated.h"

UCLASS()
class DDSIMULATOR_API AAssetsManager : public AActor
{
	GENERATED_BODY()
	
public:
	AAssetsManager();

	virtual void BeginPlay() override;
	
	virtual void Tick( float DeltaSeconds ) override;
};
