// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "AcroRemoteResourceManager.h"

AcroRemoteResourceManager::AcroRemoteResourceManager()
{
	if (!GorbClient.Connect())
	{
		UE_LOG(LogTemp, Warning, TEXT("Connection to Gorb failed"));
	}
}

bool AcroRemoteResourceManager::LoadGames(TArray<FLevelData>& Levels)
{
	if (!GorbClient.IsConnected()) return false;
	return GorbClient.LoadGames(Levels);
}

bool AcroRemoteResourceManager::SaveGames(const TArray<FLevelData>& Levels)
{
	if (!GorbClient.IsConnected()) return false;
	return false;
}

bool AcroRemoteResourceManager::CreateNewGame(const FString & GameName, FLevelData & Level)
{
	if (!GorbClient.IsConnected()) return false;
	FGuid guid = FGuid::NewGuid();
	Level.UUID = guid.ToString();
	Level.LevelName = FName(*GameName);
	Level.LevelSegments = 0;

	return GorbClient.CreateNewGame(Level);
}

bool AcroRemoteResourceManager::SaveAcroMesh(FLevelData * LevelData, UAcroMesh* Mesh)
{
	if (!GorbClient.IsConnected()) return false;
	return GorbClient.SaveAcroMesh(LevelData->UUID, Mesh);
}

bool AcroRemoteResourceManager::LoadAcroMeshes(FLevelData * LevelData, TArray<UAcroMesh*> * Meshes)
{
	if (!GorbClient.IsConnected()) return false;
	return GorbClient.LoadAcroMeshes(LevelData->UUID, Meshes);
}
