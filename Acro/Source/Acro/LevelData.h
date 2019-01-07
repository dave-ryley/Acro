// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class ACRO_API ULevelData
{
public:
    ULevelData();
    ~ULevelData();

    const FName& GetLevelName();
    int32 GetLevelSegmentCount();
    void SetLevelName(const FString& Name);
    bool Save(const FString& filepath);
    bool Load(const FString& filepath);


private:
    int32 LevelSegments = 0;
    FName LevelName;

    void SaveLoad(FArchive& Ar);
};
