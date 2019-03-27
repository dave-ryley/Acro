// Copyright © 2018-2019 David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "IAcroResourceManager.h"
#include "AcroGorbClient.h"

class ACRO_API AcroRemoteResourceManager : public IAcroResourceManager
{
public:

	AcroRemoteResourceManager();
	virtual ~AcroRemoteResourceManager() {};
	bool LoadGames(TArray<FLevelData> &Levels) override;
	bool SaveGames(const TArray<FLevelData> &Levels) override;
	bool CreateNewGame(const FString& GameName, FLevelData &Level) override;
	FString GenerateUUID() override;
	bool SaveAcroMesh(FLevelData * LevelData, UAcroMesh* Mesh) override;
	bool DeleteAcroMesh(FLevelData * LevelData, UAcroMesh* Mesh) override;
	bool LoadAcroMeshes(FLevelData * LevelData, TArray<UAcroMesh*> * Meshes) override;
private:
	AcroGorbClient GorbClient;
};