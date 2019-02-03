// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "AcroRemoteResourceManager.h"

bool AcroRemoteResourceManager::LoadGames(TArray<FLevelData>& Levels)
{
	return GorbClient.RetrieveAllLevels(Levels);
}

bool AcroRemoteResourceManager::SaveGames(const TArray<FLevelData>& Levels)
{

	return false;
}

bool AcroRemoteResourceManager::CreateNewGame(const FString & GameName, FLevelData & Level)
{
	FGuid guid = FGuid::NewGuid();
	Level.UUID = guid.ToString();
	Level.LevelName = FName(*GameName);
	Level.LevelSegments = 0;

	return GorbClient.InsertLevelData(Level);
}
