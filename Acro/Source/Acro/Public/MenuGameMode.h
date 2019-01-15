// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LevelData.h"
#include "MenuGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelLoadedDelegate, FLevelData, levelData);

UCLASS()
class ACRO_API AMenuGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AMenuGameMode();

    UFUNCTION(BlueprintCallable, Category = "SaveLoad")
    bool LoadGames();

    UFUNCTION(BlueprintCallable, Category = "SaveLoad")
    bool CreateNewGame(const FString& GameName);

    UFUNCTION(BlueprintCallable, Category = "SaveLoad")
    bool LoadLevel(FLevelData LevelData);

    UPROPERTY(BlueprintAssignable, Category = "SaveLoad")
    FOnLevelLoadedDelegate LevelLoaded;

private:
    TArray<FLevelData> Levels;

    bool SaveLevelData(const FString& filePath, FLevelData* LevelData);
    bool LoadLevelData(const FString& filePath, FLevelData* LevelData);
    void SaveLoadLevelData(FArchive& Ar, FLevelData* LevelData);
};
