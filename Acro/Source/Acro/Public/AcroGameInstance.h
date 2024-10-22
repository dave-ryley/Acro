// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

#include "Engine/GameInstance.h"

#include "IAcroResourceManager.h"
#include "AcroLocalResourceManager.h"
#include "LevelData.h"

#include "AcroGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelLoadedDelegate, FLevelData, levelData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNoLoadResultsDelegate);

UCLASS()
class ACRO_API UAcroGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UAcroGameInstance(const FObjectInitializer & ObjectInitializer);
	void Init();

	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
	bool LoadGames(bool Creative);

	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
	bool CreateNewVersusGame(const FString& GameName);

	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
	bool CreateNewCreativeGame(const FString& GameName);

	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
	bool LoadLevel(FLevelData LevelData);

	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
	bool DeleteSessions();
	
	UFUNCTION(BlueprintCallable, Category = "Level Management")
	const FName& GetCurrentLevelName();

	UFUNCTION(BlueprintCallable, Category = "Level Management")
	const int32 GetCurrentLevelExtensions();

	UFUNCTION(BlueprintCallable, Category = "Level Management")
	void LoadMenuUI(); // TODO: Set up the menu UI through C++

	UFUNCTION(BlueprintCallable, Category = "Level Management")
	void LoadMapCallback();

	UFUNCTION(BlueprintCallable, Category = "Map Transition")
	void ReturnToMainMenu();

	void SetCurrentLevelData(FLevelData LevelData);
	FLevelData* GetCurrentLevelData();

	UPROPERTY(BlueprintAssignable, Category = "SaveLoad")
	FOnLevelLoadedDelegate LevelLoaded;

	UPROPERTY(BlueprintAssignable, Category = "SaveLoad")
	FOnNoLoadResultsDelegate NoLoadResults;

private:
	bool CreativeGame = false;
	TUniquePtr<IAcroResourceManager> ResourceManagerPtr;
	FLevelData CurrentLevelData;
	TSubclassOf<class UUserWidget> MenuUI;
	TArray<FLevelData> CreativeGames;
	TArray<FLevelData> VersusGames;

	void CreateSession(FName UUID);

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	//FMeshManager MeshManager;

	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionsComplete(bool Success);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
};
