// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "MenuGameMode.h"
#include "AcroGameInstance.h"
#include "Engine/GameEngine.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/HAL/FileManagerGeneric.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformFile.h"
#include "Runtime/Core/Public/Serialization/BufferArchive.h"
#include "Runtime/Core/Public/Serialization/MemoryReader.h"
#include "../Public/AcroDefinitions.h"

AMenuGameMode::AMenuGameMode()
{

}

bool AMenuGameMode::LoadGames()
{
    FString SaveDirectoryPath = SAVE_DIRECTORY_PATH;
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    FString SaveDirectoryAbsolutePath = PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*SaveDirectoryPath);

    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, SaveDirectoryPath);
    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, SaveDirectoryAbsolutePath);

    if (!PlatformFile.DirectoryExists(*SaveDirectoryAbsolutePath))
    {
        PlatformFile.CreateDirectory(*SaveDirectoryAbsolutePath);
        if (PlatformFile.DirectoryExists(*SaveDirectoryAbsolutePath))
        {
            return true;
        }
        return false;
    }
    else
    {
        TArray<FString> SaveFiles;
        FString FinalPath = SaveDirectoryAbsolutePath + "/*";
        FFileManagerGeneric::Get().FindFiles(SaveFiles, *FinalPath, false, true);
        if (SaveFiles.Num() == 0)
        {
            return false;
        }
        else
        {
            for (int i = 0; i < SaveFiles.Num(); i++)
            {
                FString FullDirectoryPath = SaveDirectoryAbsolutePath + "/" + SaveFiles[i];
                Levels.Add(FLevelData());
                LoadLevelData(*FullDirectoryPath, &(Levels.Last()));
                LevelLoaded.Broadcast(Levels.Last());
            }
        }
    }
    return true;
}

bool AMenuGameMode::CreateNewGame(const FString& GameName)
{
    FString SaveDirectoryPath = SAVE_DIRECTORY_PATH;
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    FString SaveDirectoryAbsolutePath = PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*SaveDirectoryPath);

    FString SaveDirectoryName = FString(GameName);
    SaveDirectoryName.RemoveSpacesInline();

    // Check for SaveGame Directory
    if (!PlatformFile.DirectoryExists(*SaveDirectoryAbsolutePath))
    {
        PlatformFile.CreateDirectory(*SaveDirectoryAbsolutePath);
        if (!PlatformFile.DirectoryExists(*SaveDirectoryAbsolutePath))
        {
            return false;
        }
    }

    // Check for Level Directory
    FString FullDirectoryPath = SaveDirectoryAbsolutePath + "/" + SaveDirectoryName;
    FString UUID;
    if (PlatformFile.DirectoryExists(*FullDirectoryPath))
    {
        bool directoryExists = true;
        for (int i = 1; directoryExists; i++)
        {
            FString path = FullDirectoryPath + FString::FromInt(i);
            directoryExists = PlatformFile.DirectoryExists(*path);
            if (!directoryExists)
            {
                UUID = SaveDirectoryName + FString::FromInt(i);
                PlatformFile.CreateDirectory(*path);
                if (PlatformFile.DirectoryExists(*path))
                {
                    FullDirectoryPath = path;
                }
                else
                {
                    return false;
                }
            }
        }
    }
    else
    {
        UUID = SaveDirectoryName;
        PlatformFile.CreateDirectory(*FullDirectoryPath);
        if (!PlatformFile.DirectoryExists(*FullDirectoryPath))
        {
            return false;
        }
    }

    // Create New LevelData and Save

    Levels.Add(FLevelData());
    Levels.Last().LevelName = FName(*GameName);
    Levels.Last().UUID = UUID;
    SaveLevelData(*FullDirectoryPath, &(Levels.Last()));

    UAcroGameInstance* GameInstance = Cast<UAcroGameInstance>(GetGameInstance());
    GameInstance->SetCurrentLevelData(&(Levels.Last()));

    return true;
}

bool AMenuGameMode::LoadLevel(FLevelData LevelData)
{
    UAcroGameInstance* GameInstance = Cast<UAcroGameInstance>(GetGameInstance());
    GameInstance->SetCurrentLevelData(&LevelData);
    return true;
}

bool AMenuGameMode::SaveLevelData(const FString & filePath, FLevelData* LevelData)
{
    FBufferArchive ByteArrayBuffer;
    SaveLoadLevelData(ByteArrayBuffer, LevelData);
    if (ByteArrayBuffer.Num() == 0)
    {
        return false;
    }

    FString fullFilePath = FString::Printf(TEXT("%s/levelData.als"), *filePath);

    bool success = FFileHelper::SaveArrayToFile(ByteArrayBuffer, *fullFilePath);

    ByteArrayBuffer.FlushCache();
    ByteArrayBuffer.Empty();

    return success;
}

bool AMenuGameMode::LoadLevelData(const FString & filePath, FLevelData* LevelData)
{
    TArray<uint8> BinaryArray;
    FString fullFilePath = FString::Printf(TEXT("%s/levelData.als"), *filePath);
    if (!FFileHelper::LoadFileToArray(BinaryArray, *fullFilePath))
    {
        return false;
    }
    if (BinaryArray.Num() <= 0)
    {
        return false;
    }
    FMemoryReader FromBinary = FMemoryReader(BinaryArray, true);
    FromBinary.Seek(0);
    SaveLoadLevelData(FromBinary, LevelData);

    FromBinary.FlushCache();

    BinaryArray.Empty();
    FromBinary.Close();

    return true;
}

void AMenuGameMode::SaveLoadLevelData(FArchive& Ar, FLevelData* LevelData)
{
    Ar << LevelData->UUID;
    Ar << LevelData->LevelName;
    Ar << LevelData->LevelSegments;
}
