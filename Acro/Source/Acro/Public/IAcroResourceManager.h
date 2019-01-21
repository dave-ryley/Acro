// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "LevelData.h"

class ACRO_API IAcroResourceManager
{
public:
	virtual bool LoadGames(TArray<FLevelData> &Levels) = 0;
	virtual bool SaveGames(const TArray<FLevelData> &Levels) = 0;
	virtual bool CreateNewGame(const FString& GameName, FLevelData &Level) = 0;
};
