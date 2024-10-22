// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "AcroPlayerController.h"
#include "../Public/AcroGameMode.h"
#include "../Public/AcroDefinitions.h"

AAcroPlayerController::AAcroPlayerController() :
    PlayCharacter(nullptr),
    CreativePawn(nullptr)
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}

void AAcroPlayerController::EnterCreativeMode(FLevelSegment * LevelSegment, FVector Position)
{
	UE_LOG(LogTemp, Warning, TEXT("AAcroPlayerController::EnterCreativeMode"));
    if (CreativePawn == nullptr)
    {
        CreativePawn = GetWorld()->SpawnActor<AAcroCreativePawn>(AAcroCreativePawn::StaticClass());
    }
    if (PlayCharacter != nullptr)
    {
        PlayCharacter->SetActorHiddenInGame(true);
    }

    CreativePawn->SetActorLocation(Position);
    CreativePawn->SetLevelSegment(LevelSegment);
    Possess(CreativePawn);
    CreativeModeEntered.Broadcast();
}

void AAcroPlayerController::EnterPlayMode(FVector Position)
{
	UE_LOG(LogTemp, Warning, TEXT("AAcroPlayerController::EnterPlayMode(FVector Position)"));
	if (PlayCharacter == nullptr)
    {
        AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
        if (GameMode == nullptr)
        {
            return;
        }
        AAcroGameMode* AcroGameMode = Cast<AAcroGameMode>(GameMode);
        FActorSpawnParameters SpawnParams = FActorSpawnParameters();
        SpawnParams.bNoFail = true;
        PlayCharacter = GetWorld()->SpawnActor<AAcroCharacter>(AcroGameMode->PlayerCharacterClass, FTransform(Position), SpawnParams);
    }
    else
    {
        PlayCharacter->SetActorHiddenInGame(false);
    }

    PlayCharacter->SetActorLocation(Position);
    Possess(PlayCharacter);
    PlayModeEntered.Broadcast();
}

void AAcroPlayerController::EnterTestMode(FVector Position)
{
    if (PlayCharacter == nullptr)
    {
        AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
        if (GameMode == nullptr)
        {
            return;
        }
        AAcroGameMode* AcroGameMode = Cast<AAcroGameMode>(GameMode);
        FActorSpawnParameters SpawnParams = FActorSpawnParameters();
        SpawnParams.bNoFail = true;
        PlayCharacter = GetWorld()->SpawnActor<AAcroCharacter>(AcroGameMode->PlayerCharacterClass, FTransform(Position), SpawnParams);
    }
    else
    {
        PlayCharacter->SetActorHiddenInGame(false);
    }

    PlayCharacter->SetActorLocation(Position);
    Possess(PlayCharacter);
    TestModeEntered.Broadcast();
}
