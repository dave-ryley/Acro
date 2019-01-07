// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "AcroGameInstance.h"

const FName NullName = FName();
const FName& UAcroGameInstance::GetCurrentLevelName()
{
    if(CurrentLevelData) return CurrentLevelData->GetLevelName();
    return NullName;
}

const int32 UAcroGameInstance::GetCurrentLevelExtensions()
{
    if (CurrentLevelData) return CurrentLevelData->GetLevelSegmentCount();
    return 0;
}

void UAcroGameInstance::SetCurrentLevelData(ULevelData * LevelData)
{
    if (CurrentLevelData != nullptr && CurrentLevelData != LevelData)
    {
        delete CurrentLevelData;
    }
    CurrentLevelData = LevelData;
}
