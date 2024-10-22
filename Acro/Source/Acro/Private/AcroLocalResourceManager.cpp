// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "AcroLocalResourceManager.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManagerGeneric.h"
#include "HAL/PlatformFilemanager.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"
#include "AcroDefinitions.h"

#define LEVEL_FILE TEXT("%s/levelData.als")

AcroLocalResourceManager::~AcroLocalResourceManager()
{

}

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
	if (PlatformFile.DirectoryExists(*FullDirectoryPath))
	{
		bool directoryExists = true;
		for (int i = 1; directoryExists; i++)
		{
			FString path = FullDirectoryPath + FString::FromInt(i);
			directoryExists = PlatformFile.DirectoryExists(*path);
			if (!directoryExists)
			{
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
		PlatformFile.CreateDirectory(*FullDirectoryPath);
		if (!PlatformFile.DirectoryExists(*FullDirectoryPath))
		{
			return false;
		}
	}

	Level.LevelName = FName(*GameName);
	Level.UUID = GenerateUUID();
	SaveLevelData(*FullDirectoryPath, &Level);
	return true;
}

FString AcroLocalResourceManager::GenerateUUID()
{
	return FGuid::NewGuid().ToString();
}

bool AcroLocalResourceManager::SaveAcroMesh(FLevelData * LevelData, UAcroMesh* Mesh)
{
	if(Mesh->MeshActor != NULL)
	{
		uint8 CenterPositionsCount = Mesh->MeshActor->CenterPositions.Num();
		MeshDataBuffer << CenterPositionsCount;
		for (int i = 0; i < Mesh->MeshActor->CenterPositions.Num(); i++)
		{
			MeshDataBuffer << Mesh->MeshActor->CenterPositions[i];
		}
	}
	return FFileHelper::SaveArrayToFile(MeshDataBuffer, *FullMeshPath);
}

bool AcroLocalResourceManager::DeleteAcroMesh(FLevelData * LevelData, UAcroMesh* Mesh)
{
	UE_LOG(LogTemp, Warning, TEXT("TODO: Delete Mesh from Disk!"));
	return true;
}

void LoadAcroMesh(UAcroMesh* Mesh, FArchive & Ar)
{
	uint8 CenterPositionsCount;
	Ar << CenterPositionsCount;
	if (Mesh->CenterPositions.Num() < CenterPositionsCount)
	{
		Mesh->CenterPositions.Reserve(CenterPositionsCount);
		for (int i = 0; i < CenterPositionsCount; i++)
		{
			Mesh->CenterPositions.Add(FVector::ZeroVector);
		}
	}
	for (int i = 0; i < Mesh->CenterPositions.Num(); i++)
	{
		Ar << Mesh->CenterPositions[i];
	}
}

bool AcroLocalResourceManager::LoadAcroMeshes(FLevelData * LevelData, TArray<UAcroMesh*> * Meshes)
{
	MeshDataBuffer.FlushCache();
	MeshDataBuffer.Empty();
	FString DirectoryPath = SAVE_DIRECTORY_PATH;
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString DirectoryPostfix = FString::Printf(TEXT("/%s/"), *(LevelData->UUID));
	FString SaveDirectoryAbsolutePath = PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*DirectoryPath);
	if (!PlatformFile.DirectoryExists(*SaveDirectoryAbsolutePath))
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Cannot find save path: " + DirectoryPath);
		return false;
	}
	SaveDirectoryAbsolutePath += DirectoryPostfix;
	FullMeshPath = SaveDirectoryAbsolutePath + "meshes.acro";

	TArray<uint8> BinaryArray;
	if (FFileHelper::LoadFileToArray(BinaryArray, *FullMeshPath) && BinaryArray.Num() > 0)
	{
		FMemoryReader FromBinary = FMemoryReader(BinaryArray, true);
		FromBinary.Seek(0);

		while(!FromBinary.AtEnd())
		{
			UAcroMesh * Mesh = NewObject<UAcroMesh>();
			LoadAcroMesh(Mesh, FromBinary);
			LoadAcroMesh(Mesh, MeshDataBuffer); // Loading into Save buffer
			Meshes->Add(Mesh);
		}

		BinaryArray.Empty();
		FromBinary.FlushCache();
		FromBinary.Close();
	}
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
