// Copyright © 2018-2019 David Ryan. All rights reserved.

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
	bool SaveAcroMesh(FString & LevelUuid, UAcroMesh * Mesh);
	bool LoadAcroMeshes(FString & LevelUuid, TArray<UAcroMesh*> * Meshes);
	bool DeleteAcroMesh(FString & LevelUuid, FString & MeshUuid);

private:
	class AcroGorbClientImpl;
	TUniquePtr<AcroGorbClientImpl> GorbClient;
};
