// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
#include "PowerDataAsset.h"

void UPowerDataAsset::GetPowerByUID(int32 UID, FPowerData& Data)
{
  for (int32 i = 0; i != items.Num(); i++) {
    if (items[i].UID == UID) {
      Data = items[i];
      break;
    }
  }
}

bool UPowerDataAsset::CheckExists(int32 UID)
{
  for (int32 i = 0; i != items.Num(); i++) {
    if (items[i].UID == UID) {
      return true;
    }
  }
  return false;
}
