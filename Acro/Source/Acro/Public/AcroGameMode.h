// Copyright � 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AcroCharacter.h"

#include "AcroGameMode.generated.h"

UCLASS(minimalapi)
class AAcroGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAcroGameMode();

	virtual void StartPlay() override;

	virtual void SaveMesh(UAcroMesh* Mesh);
	virtual void DeleteMesh(UAcroMesh* Mesh);

	TSubclassOf<AAcroCharacter> PlayerCharacterClass;

protected:
	TArray<UAcroMesh*> Meshes;
};
