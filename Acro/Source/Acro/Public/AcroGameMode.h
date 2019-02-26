// Copyright � 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LevelSegment.h"
#include "AcroCheckpointTrigger.h"
#include "AcroCharacter.h"
#include "IAcroResourceManager.h"
#include "AcroGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnValidateLevelDelegate);

UCLASS(minimalapi)
class AAcroGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAcroGameMode();

	// Loads the Level Segments from disk
	virtual void InitGame( const FString & MapName, const FString & Options, FString & ErrorMessage) override;

	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
	bool SaveLevelData();

	virtual void StartPlay() override;

	void ValidateLevelSegment();

	void SaveMesh(UAcroMesh* Mesh);

	UFUNCTION(BlueprintCallable, Category = "Game States")
	void EnterCreativeMode();
	UFUNCTION(BlueprintCallable, Category = "Game States")
	void EnterPlayMode();
	UFUNCTION(BlueprintCallable, Category = "Game States")
	void EnterTestMode();

	UPROPERTY(BlueprintAssignable, Category = "Game States")
	FOnValidateLevelDelegate LevelSegmentValidated;

	TArray<FLevelSegment> LevelSegments;
	FLevelSegment* CurrentLevelSegment = nullptr;

	TSubclassOf<AAcroCharacter> PlayerCharacterClass;

private:
	AAcroCheckpointTrigger* EndCheckpointTrigger = nullptr;
	AAcroCheckpointTrigger* ValidationCheckpointTrigger = nullptr;
	bool bLevelInProgress = false;
	FString FullMeshPath;
	TArray<UAcroMesh*> MeshesToLoad;

	TUniquePtr<IAcroResourceManager> ResourceManager;
};
