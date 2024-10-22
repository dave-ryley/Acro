// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "AcroCreativeGameMode.h"
#include "AcroGameInstance.h"
#include "AcroPlayerController.h"
#include "AcroLocalResourceManager.h"
#include "AcroDefinitions.h"
#include "AcroPlayerController.h"

#include "Engine/GameEngine.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformFile.h"

AAcroCreativeGameMode::AAcroCreativeGameMode() :
	AAcroGameMode(),
	ResourceManager(MakeUnique<AcroLocalResourceManager>())
{

}

void AAcroCreativeGameMode::InitGame(const FString & MapName, const FString & Options, FString & ErrorMessage)
{
	AAcroGameMode::InitGame(MapName, Options, ErrorMessage);

	UAcroGameInstance* GameInstance = Cast<UAcroGameInstance>(GetGameInstance());
	FLevelData * CurrentLevelData = GameInstance->GetCurrentLevelData();
	ResourceManager->LoadAcroMeshes(CurrentLevelData, &Meshes);

	// LevelSegments.Empty(); // TODO: Make sure this is done on Game ending.

	// FString DirectoryPath = FString::Printf(TEXT("%s/%s/"), *SAVE_DIRECTORY_PATH, *(CurrentLevelData->UUID));
	// IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	// FString SaveDirectoryAbsolutePath = PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*DirectoryPath);


	// for (int i = 0; i < CurrentLevelData->LevelSegments; i++)
	// {
	// 	TArray<uint8> BinaryArray;
	// 	FString FullPath = SaveDirectoryAbsolutePath + "seg_" + FString::FromInt(i) + ".ass";
	// 	if (!FFileHelper::LoadFileToArray(BinaryArray, *FullPath))
	// 	{
	// 		break;
	// 	}
	// 	if (BinaryArray.Num() <= 0)
	// 	{
	// 		break;
	// 	}
	// 	FMemoryReader FromBinary = FMemoryReader(BinaryArray, true);
	// 	FromBinary.Seek(0);

	// 	LevelSegments.Add(FLevelSegment());
	// 	LevelSegments[i].SaveLoadSegment(FromBinary);

	// 	BinaryArray.Empty();

	// 	FromBinary.FlushCache();
	// 	FromBinary.Close();
	// }

}

void AAcroCreativeGameMode::StartPlay()
{
	AAcroGameMode::StartPlay();
	UWorld* World = GetWorld();
	for (int i = 0; i < Meshes.Num(); i++)
	{
		Meshes[i]->ConstructLoadedMesh(World);
	}
	//if (LevelSegments.Num() == 0)
	//{
	//    EnterCreativeMode();
	//}
	//else
	//{
	//    UWorld* World = GetWorld();
	//    for (int i = 0; i < LevelSegments.Num(); i++)
	//    {
	//        LevelSegments[i].ConstructSegment(World);
	//    }
	//    EnterPlayMode();
	//}
	//EnterPlayMode();
}

void AAcroCreativeGameMode::ValidateLevelSegment()
{
	LevelSegmentValidated.Broadcast();
}

void AAcroCreativeGameMode::EnterCreativeMode()
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

void AAcroCreativeGameMode::EnterPlayMode()
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

void AAcroCreativeGameMode::EnterTestMode()
{
	FVector Position = FVector(LEVEL_RADIUS, 0.0, 50.0 + LEVEL_HEIGHT * (LevelSegments.Num() - 1));
	float rotationAmount = ((LevelSegments.Num() - 1) * LEVEL_WIDTH) / LEVEL_CIRCUMFERENCE * 360.f;
	Position = Position.RotateAngleAxis(rotationAmount, FVector::UpVector);

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC != nullptr)
	{
		AAcroPlayerController* PlayerController = Cast<AAcroPlayerController>(PC);
		PlayerController->EnterTestMode(Position);
	}
}

bool AAcroCreativeGameMode::SaveLevelData()
{
	UAcroGameInstance* GameInstance = Cast<UAcroGameInstance>(GetGameInstance());
	FLevelData * CurrentLevelData = GameInstance->GetCurrentLevelData();
	FString DirectoryPath = SAVE_DIRECTORY_PATH;
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString DirectoryPostfix = FString::Printf(TEXT("/%s/"), *(CurrentLevelData->UUID));
	FString SaveDirectoryAbsolutePath = PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*DirectoryPath);
	SaveDirectoryAbsolutePath += DirectoryPostfix;
	if (!PlatformFile.DirectoryExists(*SaveDirectoryAbsolutePath))
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Cannot find save path: " + DirectoryPath);
		return false;
	}

	for (int i = 0; i < LevelSegments.Num(); i++)
	{
		FBufferArchive ByteArrayBuffer;
		LevelSegments[i].SaveLoadSegment(ByteArrayBuffer);
		if (ByteArrayBuffer.Num() == 0)
		{
			return false;
		}
		FString FullPath = SaveDirectoryAbsolutePath + "seg_" + FString::FromInt(i) + ".ass";
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

void AAcroCreativeGameMode::SaveMesh(UAcroMesh* Mesh)
{
	AAcroGameMode::SaveMesh(Mesh);
	UAcroGameInstance* GameInstance = Cast<UAcroGameInstance>(GetGameInstance());
	FLevelData * CurrentLevelData = GameInstance->GetCurrentLevelData();
	ResourceManager->SaveAcroMesh(CurrentLevelData, Mesh);
}

void AAcroCreativeGameMode::DeleteMesh(UAcroMesh* Mesh)
{
	AAcroGameMode::DeleteMesh(Mesh);
	UAcroGameInstance* GameInstance = Cast<UAcroGameInstance>(GetGameInstance());
	FLevelData * CurrentLevelData = GameInstance->GetCurrentLevelData();
	ResourceManager->DeleteAcroMesh(CurrentLevelData, Mesh);
}


