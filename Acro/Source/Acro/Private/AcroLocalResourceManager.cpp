// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "AcroLocalResourceManager.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManagerGeneric.h"
#include "HAL/PlatformFilemanager.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"

#define LEVEL_FILE TEXT("%s/levelData.als")
const FString SAVE_DIRECTORY_PATH = FString("SaveGames");

bool AcroLocalResourceManager::LoadGames(TArray<FLevelData>& Levels)
{
	FString SaveDirectoryPath = SAVE_DIRECTORY_PATH;
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString SaveDirectoryAbsolutePath = PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*SaveDirectoryPath);

	if (!PlatformFile.DirectoryExists(*SaveDirectoryAbsolutePath))
	{
		PlatformFile.CreateDirectory(*SaveDirectoryAbsolutePath);
		if (PlatformFile.DirectoryExists(*SaveDirectoryAbsolutePath))
		{
			// Nothing to load, directory created.
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
				if (!LoadLevelData(*FullDirectoryPath, &(Levels.Last())))
				{
					return false;
				}
			}
		}
	}
	return true;
}

bool AcroLocalResourceManager::SaveGames(const TArray<FLevelData>& Levels)
{
	return false;
}

bool AcroLocalResourceManager::CreateNewGame(const FString & GameName, FLevelData & Level)
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

	Level.LevelName = FName(*GameName);
	Level.UUID = UUID;
	SaveLevelData(*FullDirectoryPath, &Level);
	return true;
}

bool AcroLocalResourceManager::SaveLevelData(const FString & FullDirectoryPath, FLevelData * LevelData)
{
	FBufferArchive ByteArrayBuffer;
	SaveLoadLevelData(ByteArrayBuffer, LevelData);
	if (ByteArrayBuffer.Num() == 0)
	{
		return false;
	}

	FString FullFilePath = FString::Printf(LEVEL_FILE, *FullDirectoryPath);

	bool Success = FFileHelper::SaveArrayToFile(ByteArrayBuffer, *FullFilePath);

	ByteArrayBuffer.FlushCache();
	ByteArrayBuffer.Empty();

	return Success;
}

bool AcroLocalResourceManager::LoadLevelData(const FString & FullDirectoryPath, FLevelData * LevelData)
{
	TArray<uint8> BinaryArray;
	FString FullFilePath = FString::Printf(LEVEL_FILE, *FullDirectoryPath);
	if (!FFileHelper::LoadFileToArray(BinaryArray, *FullFilePath))
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

void AcroLocalResourceManager::SaveLoadLevelData(FArchive & Ar, FLevelData * LevelData)
{
	Ar << LevelData->UUID;
	Ar << LevelData->LevelName;
	Ar << LevelData->LevelSegments;
}
