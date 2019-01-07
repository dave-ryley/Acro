// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "MenuGameMode.h"
#include "Engine/GameEngine.h"
#include "Runtime/Core/Public/HAL/FileManagerGeneric.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformFile.h"
#include "AcroGameInstance.h"
#include "LevelData.h"

#define printf(text, ...) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, FString::Printf(TEXT(text), ##__VA_ARGS__))

#define SAVE_DIRECTORY_PATH "SaveGames"

AMenuGameMode::AMenuGameMode()
{

}

bool AMenuGameMode::LoadGames()
{
    FString SaveDirectoryPath = FString(TEXT(SAVE_DIRECTORY_PATH));
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    FString SaveDirectoryAbsolutePath = PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*SaveDirectoryPath);

    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, SaveDirectoryPath);
    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, SaveDirectoryAbsolutePath);

    if (!PlatformFile.DirectoryExists(*SaveDirectoryAbsolutePath))
    {
        printf("Cannot find Directory, Creating.");
        PlatformFile.CreateDirectory(*SaveDirectoryAbsolutePath);
        if (PlatformFile.DirectoryExists(*SaveDirectoryAbsolutePath))
        {
            printf("Directory Creation Successful.");
        }
        else
        {
            printf("Directory Creation Failed.");
        }
        return false;
    }
    else
    {
        TArray<FString> SaveFiles;
        FString FinalPath = SaveDirectoryAbsolutePath + "/*";
        FFileManagerGeneric::Get().FindFiles(SaveFiles, *FinalPath, false, true);
        printf("Found Directory.");
        if (SaveFiles.Num() == 0)
        {
            printf("No save files exist.");
            return false;
        }
        else
        {
            printf("%d save files exist.", SaveFiles.Num());
            for (int i = 0; i < SaveFiles.Num(); i++)
            {
                if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, SaveFiles[i]);
                // Validate each file and load their resources. PNG, etc...
            }
        }
    }
    return true;
}

bool AMenuGameMode::CreateNewGame(const FString& GameName)
{
    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, GameName);

    FString SaveDirectoryPath = FString(TEXT(SAVE_DIRECTORY_PATH));
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    FString SaveDirectoryAbsolutePath = PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*SaveDirectoryPath);

    FString SaveDirectoryName = FString(GameName);
    SaveDirectoryName.RemoveSpacesInline();

    // Check for SaveGame Directory
    if (!PlatformFile.DirectoryExists(*SaveDirectoryAbsolutePath))
    {
        printf("Cannot find Directory, Creating.");
        PlatformFile.CreateDirectory(*SaveDirectoryAbsolutePath);
        if (PlatformFile.DirectoryExists(*SaveDirectoryAbsolutePath))
        {
            printf("Directory Creation Successful.");
        }
        else
        {
            printf("Directory Creation Failed.");
            return false;
        }
    }

    // Check for Level Directory
    FString FullDirectoryPath = SaveDirectoryAbsolutePath + "/" + SaveDirectoryName;
    if (PlatformFile.DirectoryExists(*FullDirectoryPath))
    {
        bool directoryExists = true;
        for (int i = 1; directoryExists; i++)
        {
            FString path = FString(*FullDirectoryPath) + FString::FromInt(i);
            directoryExists = PlatformFile.DirectoryExists(*path);
            if (!directoryExists)
            {
                PlatformFile.CreateDirectory(*path);
                if (PlatformFile.DirectoryExists(*path))
                {
                    printf("Directory Creation Successful.");
                    FullDirectoryPath = path;
                }
                else
                {
                    printf("Directory Creation Failed.");
                    return false;
                }
            }
        }
    }
    else
    {
        PlatformFile.CreateDirectory(*FullDirectoryPath);
        if (PlatformFile.DirectoryExists(*FullDirectoryPath))
        {
            printf("Directory Creation Successful.");
        }
        else
        {
            printf("Directory Creation Failed.");
            return false;
        }
    }

    // Create New LevelData and Save

    ULevelData* CurrentLevelData = new ULevelData();
    CurrentLevelData->SetLevelName(*GameName);
    CurrentLevelData->Save(*FullDirectoryPath);

    UAcroGameInstance* GameInstance = Cast<UAcroGameInstance>(GetGameInstance());
    GameInstance->SetCurrentLevelData(CurrentLevelData);

    return true;
}