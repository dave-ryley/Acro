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


UCLASS()
class ACRO_API UAcroGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UAcroGameInstance(const FObjectInitializer & ObjectInitializer);
	void Init();

	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
	bool LoadGames();

	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
	bool CreateNewGame(const FString& GameName);

	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
	bool LoadLevel(FLevelData LevelData);

	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
	bool DeleteSessions();
	
	UFUNCTION(BlueprintCallable, Category = "Level Management")
	const FName& GetCurrentLevelName();

	UFUNCTION(BlueprintCallable, Category = "Level Management")
	const int32 GetCurrentLevelExtensions();

	UFUNCTION(Exec, BlueprintCallable, Category = "Level Management")
	void LoadMenuUI(); // TODO: Set up the menu UI through C++

	UFUNCTION(Exec, BlueprintCallable, Category = "Level Management")
	void LoadMapCallback();

	void SetCurrentLevelData(FLevelData* LevelData);
	FLevelData* GetCurrentLevelData();

	UPROPERTY(BlueprintAssignable, Category = "SaveLoad")
	FOnLevelLoadedDelegate LevelLoaded;

private:
	TUniquePtr<IAcroResourceManager> ResourceManagerPtr;
	FLevelData CurrentLevelData;
	TSubclassOf<class UUserWidget> MenuUI;
	TArray<FLevelData> Levels;

	void CreateSession(FName UUID);

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionsComplete(bool Success);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
};
