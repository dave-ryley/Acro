// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "LevelData.h"
#include "AcroMesh.h"

/**
 *
 */
	class ACRO_API AcroGorbClient
{
public:
	AcroGorbClient();
	~AcroGorbClient();

	bool Connect();
	bool IsConnected();

	bool CreateNewGame(FLevelData & Data);
	bool LoadGames(TArray<FLevelData>& Levels);
	bool SaveAcroMesh(FString & Uuid, UAcroMesh * Mesh);
	bool LoadAcroMeshes(FString & Uuid, TArray<UAcroMesh*> * Meshes);

private:
	class AcroGorbClientImpl;
	TUniquePtr<AcroGorbClientImpl> GorbClient;
};