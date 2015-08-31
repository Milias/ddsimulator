// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "MapPlayerPawn.generated.h"

UCLASS()
class DDSIMULATOR_API AMapPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	AMapPlayerPawn();

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Map)
  USpringArmComponent * SpringArm;

  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Map)
  UCameraComponent * PlayerCamera;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
  UStaticMeshComponent * MovingEntityMesh;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Entity)
  UMaterial * TranslucidMaterial;

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
};
