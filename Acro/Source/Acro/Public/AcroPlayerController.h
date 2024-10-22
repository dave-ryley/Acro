// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LevelSegment.h"
#include "AcroCharacter.h"
#include "AcroCreativePawn.h"
#include "AcroPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnterCreativeModeDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnterTestModeDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnterPlayModeDelegate);

UCLASS()
class ACRO_API AAcroPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AAcroPlayerController();

    void EnterCreativeMode(FLevelSegment* LevelSegment, FVector Position);
    void EnterPlayMode(FVector Position);
    void EnterTestMode(FVector Position);

    UPROPERTY(BlueprintAssignable, Category = "Game States")
    FOnEnterCreativeModeDelegate CreativeModeEntered;

    UPROPERTY(BlueprintAssignable, Category = "Game States")
    FOnEnterTestModeDelegate TestModeEntered;

    UPROPERTY(BlueprintAssignable, Category = "Game States")
    FOnEnterPlayModeDelegate PlayModeEntered;

private:
    AAcroCharacter* PlayCharacter;
    AAcroCreativePawn* CreativePawn;
};
