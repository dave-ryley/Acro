// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "AcroGameInstance.h"

const FName NullName = FName();
const FName& UAcroGameInstance::GetCurrentLevelName()
{
    return CurrentLevelData.LevelName;
}

const int32 UAcroGameInstance::GetCurrentLevelExtensions()
{
    return CurrentLevelData.LevelSegments;
}

void UAcroGameInstance::SetCurrentLevelData(FLevelData& LevelData)
{
    CurrentLevelData = LevelData;
}
