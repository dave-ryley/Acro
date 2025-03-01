// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "LevelData.generated.h"

USTRUCT(BlueprintType) // BlueprintType to make accessible for UI
struct ACRO_API FLevelData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FName LevelName;

    UPROPERTY(BlueprintReadOnly)
    int32 LevelSegments = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 ActivePlayers;

    FString UUID;
	FString SessionID;
	int32 SearchResultIndex;
};
