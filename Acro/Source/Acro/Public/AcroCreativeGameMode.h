// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AcroGameMode.h"
#include "LevelSegment.h"
#include "AcroCheckpointTrigger.h"
#include "IAcroResourceManager.h"
#include "AcroCreativeGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnValidateLevelDelegate);

UCLASS()
class ACRO_API AAcroCreativeGameMode : public AAcroGameMode
{
	GENERATED_BODY()
	
public:
	AAcroCreativeGameMode();
	// Loads the Level Segments from disk
	virtual void InitGame(const FString & MapName, const FString & Options, FString & ErrorMessage) override;
	virtual void StartPlay() override;

	virtual void SaveMesh(UAcroMesh* Mesh) override;
	virtual void DeleteMesh(UAcroMesh* Mesh) override;

	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
	bool SaveLevelData();

	void ValidateLevelSegment();

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

private:
	TUniquePtr<IAcroResourceManager> ResourceManager;
	AAcroCheckpointTrigger* EndCheckpointTrigger = nullptr;
	AAcroCheckpointTrigger* ValidationCheckpointTrigger = nullptr;
	bool bLevelInProgress = false;
	FString FullMeshPath;
};
