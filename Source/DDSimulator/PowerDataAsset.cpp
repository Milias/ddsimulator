// Fill out your copyright notice in the Description page of Project Settings.

#include "DDSimulator.h"
#include "PowerDataAsset.h"

FPowerData& UPowerDataAsset::GetPowerByUID(int32 UID)
{
  for (int32 i = 0; i != items.Num(); i++) {
    if (items[i].UID == UID) {
      return items[i];
    }
  } return items[0];
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
