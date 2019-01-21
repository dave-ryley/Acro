// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IAcroResourceManager.h"

class ACRO_API AcroLocalResourceManager : public IAcroResourceManager
{
public:
	bool LoadGames(TArray<FLevelData> &Levels) override;
	bool SaveGames(const TArray<FLevelData> &Levels) override;
	bool CreateNewGame(const FString& GameName, FLevelData &Level) override;

private:
	bool SaveLevelData(const FString & FullDirectoryPath, FLevelData* LevelData);
	bool LoadLevelData(const FString & FullDirectoryPath, FLevelData* LevelData);
	void SaveLoadLevelData(FArchive& Ar, FLevelData* LevelData);
};
