// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "Acro.h"
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
	bool SaveAcroMesh(FLevelData * LevelData, UAcroMesh* Mesh) override;
	bool LoadAcroMeshes(FLevelData * LevelData, TArray<UAcroMesh*> * Meshes) override;
private:
	AcroGorbClient GorbClient;
};