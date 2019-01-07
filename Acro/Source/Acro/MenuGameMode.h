// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MenuGameMode.generated.h"

/**
 * 
 */
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
};
