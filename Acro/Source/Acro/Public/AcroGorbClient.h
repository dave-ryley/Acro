// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "LevelData.h"

/**
 * 
 */
class ACRO_API AcroGorbClient
{
public:
	AcroGorbClient();
	~AcroGorbClient();

	bool IsConnected();

	bool InsertLevelData(FLevelData & Data);

	bool RetrieveAllLevels(TArray<FLevelData>& Levels);

private:
	class AcroGorbClientImpl;
	TUniquePtr<AcroGorbClientImpl> GorbClient;
};
