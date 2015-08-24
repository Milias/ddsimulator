// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "UnrealNetwork.h"
#include "UnrealMath.h"
#include "SceneViewport.h"
#include <algorithm>
#include <json/json.h>
#include <lodepng/lodepng.h>

#define COLLISION_MAP_TILE		        ECollisionChannel::ECC_GameTraceChannel1
#define COLLISION_MAP_BASIC_ENTITY		ECollisionChannel::ECC_GameTraceChannel2

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,text)

//Map Log
DECLARE_LOG_CATEGORY_EXTERN(MapLog, Log, All);
