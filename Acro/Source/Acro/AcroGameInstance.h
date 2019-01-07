// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LevelData.h"
#include "AcroGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ACRO_API UAcroGameInstance : public UGameInstance
{
    GENERATED_BODY()
    
public:
    
    UFUNCTION(BlueprintCallable, Category = "Level Management")
    const FName& GetCurrentLevelName();

    UFUNCTION(BlueprintCallable, Category = "Level Management")
    const int32 GetCurrentLevelExtensions();

    void SetCurrentLevelData(FLevelData& LevelData);

private:
    FLevelData CurrentLevelData;
};
