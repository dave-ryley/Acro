// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "AcroGameMode.h"
#include "../Public/AcroCharacter.h"
#include "AcroGameInstance.h"
#include "Engine/GameEngine.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformFile.h"
#include "../Public/AcroCheckpointTrigger.h"
#include "../Public/AcroPlayerState.h"
#include "../Public/AcroPlayerController.h"
#include "../Public/AcroDefinitions.h"

AAcroGameMode::AAcroGameMode()
{
    static ConstructorHelpers::FClassFinder<AAcroCharacter> PlayerPawnBPClass(TEXT("/Game/SideScrollerCPP/Blueprints/SideScrollerCharacter"));
    if (PlayerPawnBPClass.Succeeded())
    {
        PlayerCharacterClass = PlayerPawnBPClass.Class;
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }
    PlayerStateClass = AAcroPlayerState::StaticClass();
    PlayerControllerClass = AAcroPlayerController::StaticClass();
}

void AAcroGameMode::InitGame(const FString & MapName, const FString & Options, FString & ErrorMessage)
{
    AGameModeBase::InitGame(MapName, Options, ErrorMessage);

    UAcroGameInstance* GameInstance = Cast<UAcroGameInstance>(GetGameInstance());

    LevelSegments.Empty(); // TODO: Make sure this is done on Game ending.
    
    FLevelData * CurrentLevelData = GameInstance->GetCurrentLevelData();
    FString DirectoryPath = FString::Printf(TEXT("%s/%s/"), *SAVE_DIRECTORY_PATH, *(CurrentLevelData->UUID));
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    FString SaveDirectoryAbsolutePath = PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*DirectoryPath);
    if (!PlatformFile.DirectoryExists(*SaveDirectoryAbsolutePath))
    {
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Cannot find save path: " + DirectoryPath);
        return;
    }

    for (int i = 0; i < CurrentLevelData->LevelSegments; i++)
    {
        TArray<uint8> BinaryArray;
        FString FullPath = SaveDirectoryAbsolutePath + "seg_" + FString::FromInt(i) + ".ass";
        if (!FFileHelper::LoadFileToArray(BinaryArray, *FullPath))
        {
            break;
        }
        if (BinaryArray.Num() <= 0)
        {
            break;
        }
        FMemoryReader FromBinary = FMemoryReader(BinaryArray, true);
        FromBinary.Seek(0);

        LevelSegments.Add(FLevelSegment());
        LevelSegments[i].SaveLoadSegment(FromBinary);

        BinaryArray.Empty();

        FromBinary.FlushCache();
        FromBinary.Close();
    }
}

bool AAcroGameMode::SaveLevelData()
{
    UAcroGameInstance* GameInstance = Cast<UAcroGameInstance>(GetGameInstance());
    FLevelData * CurrentLevelData = GameInstance->GetCurrentLevelData();
    FString DirectoryPath = SAVE_DIRECTORY_PATH;
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    FString DirectoryPostfix = FString::Printf(TEXT("/%s/"), *(CurrentLevelData->UUID));
    FString SaveDirectoryAbsolutePath = PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*DirectoryPath);
    SaveDirectoryAbsolutePath += DirectoryPostfix;
    printf("SaveDirectoryAbsolutePath: %s", *SaveDirectoryAbsolutePath);
    if (!PlatformFile.DirectoryExists(*SaveDirectoryAbsolutePath))
    {
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Cannot find save path: " + DirectoryPath);
        return false;
    }
    printf("Saving %d Level Segments", LevelSegments.Num());

    for (int i = 0; i < LevelSegments.Num(); i++)
    {
        FBufferArchive ByteArrayBuffer;
        LevelSegments[i].SaveLoadSegment(ByteArrayBuffer);
        if (ByteArrayBuffer.Num() == 0)
        {
            return false;
        }
        FString FullPath = SaveDirectoryAbsolutePath + "seg_" + FString::FromInt(i) + ".ass";
        printf("Saving to path %s", *FullPath);
        // TODO: Warn if SaveArrayToFile returns false.
        FFileHelper::SaveArrayToFile(ByteArrayBuffer, *FullPath);
        ByteArrayBuffer.FlushCache();
        ByteArrayBuffer.Empty();
    }

    CurrentLevelData->LevelSegments = LevelSegments.Num();

    // TODO: Put the SaveLoad function in the LevelData struct
    FBufferArchive LevelDataBuffer;
    LevelDataBuffer << CurrentLevelData->UUID;
    LevelDataBuffer << CurrentLevelData->LevelName;
    LevelDataBuffer << CurrentLevelData->LevelSegments;
    if (LevelDataBuffer.Num() == 0)
    {
        return false;
    }

    FString fullFilePath = SaveDirectoryAbsolutePath + FString("levelData.als");

    // TODO: Warn if SaveArrayToFile returns false.
    FFileHelper::SaveArrayToFile(LevelDataBuffer, *fullFilePath);

    LevelDataBuffer.FlushCache();
    LevelDataBuffer.Empty();

    return true;
}

void AAcroGameMode::StartPlay()
{
    Super::StartPlay();
    if (LevelSegments.Num() == 0)
    {
        EnterCreativeMode();
    }
    else
    {
        UWorld* World = GetWorld();
        for (int i = 0; i < LevelSegments.Num(); i++)
        {
            LevelSegments[i].ConstructSegment(World);
        }
        EnterPlayMode();
    }
}

void AAcroGameMode::ValidateLevelSegment()
{
    LevelSegmentValidated.Broadcast();
}

void AAcroGameMode::EnterCreativeMode()
{
    if (!bLevelInProgress)
    {
        LevelSegments.Add(FLevelSegment());
        bLevelInProgress = true;
    }
    CurrentLevelSegment = &LevelSegments.Last();
    if (EndCheckpointTrigger != nullptr)
    {
        EndCheckpointTrigger->SetActorHiddenInGame(true);
    }
    if (ValidationCheckpointTrigger == nullptr)
    {
        ValidationCheckpointTrigger = GetWorld()->SpawnActor<AAcroCheckpointTrigger>(AAcroCheckpointTrigger::StaticClass());
        ValidationCheckpointTrigger->SetCheckPointType(true); // TODO: Change Trigger to use inheritance
    }
    else
    {
        ValidationCheckpointTrigger->SetActorHiddenInGame(false);
    }

    FVector Position = FVector(LEVEL_RADIUS, 0.0, 50.0 + LEVEL_HEIGHT * LevelSegments.Num());
    float rotationAmount = (LevelSegments.Num() * LEVEL_WIDTH) / LEVEL_CIRCUMFERENCE * 360.f;
    Position = Position.RotateAngleAxis(rotationAmount, FVector::UpVector);

    ValidationCheckpointTrigger->SetActorLocation(Position);

    // Get PlayerController to possess creativePawn
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC != nullptr)
    {
        AAcroPlayerController* PlayerController = Cast<AAcroPlayerController>(PC);
        PlayerController->EnterCreativeMode(CurrentLevelSegment, Position);
    }

}

void AAcroGameMode::EnterPlayMode()
{
    if (EndCheckpointTrigger == nullptr)
    {
        EndCheckpointTrigger = GetWorld()->SpawnActor<AAcroCheckpointTrigger>(AAcroCheckpointTrigger::StaticClass());
        EndCheckpointTrigger->SetCheckPointType(false); // TODO: Change Trigger to use inheritance
    }
    else
    {
        EndCheckpointTrigger->SetActorHiddenInGame(false);
    }

    FVector Position = FVector(LEVEL_RADIUS, 0.0, 50.0 + LEVEL_HEIGHT * LevelSegments.Num());
    float rotationAmount = (LevelSegments.Num() * LEVEL_WIDTH) / LEVEL_CIRCUMFERENCE * 360.f;
    Position = Position.RotateAngleAxis(rotationAmount, FVector::UpVector);

    // TODO: Get Previous Save Position for Player
    FVector PlayerPosition = FVector(LEVEL_RADIUS, 0.0, 50.0);

    EndCheckpointTrigger->SetActorLocation(Position);

    // Get PlayerController to possess character
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC != nullptr)
    {
        AAcroPlayerController* PlayerController = Cast<AAcroPlayerController>(PC);
        PlayerController->EnterPlayMode(PlayerPosition);
    }
}

void AAcroGameMode::EnterTestMode()
{
    FVector Position = FVector(LEVEL_RADIUS, 0.0, 50.0 + LEVEL_HEIGHT * (LevelSegments.Num()-1));
    float rotationAmount = ((LevelSegments.Num()-1) * LEVEL_WIDTH) / LEVEL_CIRCUMFERENCE * 360.f;
    Position = Position.RotateAngleAxis(rotationAmount, FVector::UpVector);

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC != nullptr)
    {
        AAcroPlayerController* PlayerController = Cast<AAcroPlayerController>(PC);
        PlayerController->EnterTestMode(Position);
    }
}