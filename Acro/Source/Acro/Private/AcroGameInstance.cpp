// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "AcroGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/GameEngine.h"
#include "OnlineSubsystemTypes.h"
#include "OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "AcroDefinitions.h"
#include "AcroVsGameMode.h"

#include "AcroMainMenuWidget.h"
#include "GameFramework/PlayerState.h"

UAcroGameInstance::UAcroGameInstance(const FObjectInitializer & ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UAcroMainMenuWidget> MenuBPClassFinder(TEXT("/Game/UI/MainMenu"));
	if (!ensure(MenuBPClassFinder.Class != nullptr)) return;

	MenuUI = MenuBPClassFinder.Class;
	UE_LOG(LogTemp, Warning, TEXT("Found class %s"), *MenuBPClassFinder.Class->GetName());

	ResourceManagerPtr = MakeUnique<AcroLocalResourceManager>();
}

void UAcroGameInstance::Init()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Online Subsystem %s"), *OnlineSubsystem->GetSubsystemName().ToString());
		SessionInterface = OnlineSubsystem->GetSessionInterface();
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UAcroGameInstance::OnCreateSessionComplete);
		SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UAcroGameInstance::OnDestroySessionComplete);
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UAcroGameInstance::OnFindSessionsComplete);
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UAcroGameInstance::OnJoinSessionComplete);
		SessionSearch = MakeShareable(new FOnlineSessionSearch());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not find an Online Subsystem"));
	}

	UE_LOG(LogTemp, Warning, TEXT("Has class %s"), *MenuUI->GetName());
}

const FName& UAcroGameInstance::GetCurrentLevelName()
{
	return CurrentLevelData.LevelName;
}

const int32 UAcroGameInstance::GetCurrentLevelExtensions()
{
	return CurrentLevelData.LevelSegments;
}

void UAcroGameInstance::LoadMenuUI()
{
	if (!ensure(MenuUI != nullptr)) return;

	UAcroMainMenuWidget* Menu = CreateWidget<UAcroMainMenuWidget>(this, MenuUI);
	if (!ensure(Menu != nullptr)) return;

	Menu->AddToViewport();

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(Menu->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = true;

	Menu->OnLoadMenu();

	if (PlayerController->HasAuthority())
	{
		APlayerState* PlayerState = PlayerController->PlayerState;
		if (!ensure(PlayerState != nullptr)) return;
		int32 PlayerId = PlayerState->PlayerId;
		UE_LOG(LogTemp, Warning, TEXT("GameInstance: Server Id: %d"), PlayerId);
	}
	else
	{
		APlayerState* PlayerState = PlayerController->PlayerState;
		if (!ensure(PlayerState != nullptr)) return;
		int32 PlayerId = PlayerState->PlayerId;
		UE_LOG(LogTemp, Warning, TEXT("GameInstance: Client Id: %d"), PlayerId);
	}
}

void UAcroGameInstance::LoadMapCallback()
{
	UE_LOG(LogTemp, Warning, TEXT("Map Loaded."));
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);

	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = true;
}

void UAcroGameInstance::SetCurrentLevelData(FLevelData LevelData)
{
	CurrentLevelData = LevelData;
}

FLevelData * UAcroGameInstance::GetCurrentLevelData()
{
	return &CurrentLevelData;
}


bool UAcroGameInstance::LoadGames(bool Creative)
{
	CreativeGame = Creative;
	if (Creative)
	{
		CreativeGames.Empty();
		bool Success = ResourceManagerPtr->LoadGames(CreativeGames);
		if (!Success) return false;
	}
	else
	{
		VersusGames.Empty();
	}

	if (SessionSearch.IsValid())
	{
		if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
		{
			UE_LOG(LogTemp, Warning, TEXT("Is LAN Query"));
			SessionSearch->bIsLanQuery = true;
		}
		else // Steam
		{
			UE_LOG(LogTemp, Warning, TEXT("Is Steam Match"));
			SessionSearch->bIsLanQuery = false;
		}
		SessionSearch->MaxSearchResults = 2000000;
		SessionSearch->TimeoutInSeconds = 120.0f;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		UE_LOG(LogTemp, Warning, TEXT("Attempting to find Sessions."));
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
		return true;
	}
	return false;
}

bool UAcroGameInstance::CreateNewVersusGame(const FString& GameName)
{
	if (SessionInterface.IsValid())
	{
		VersusGames.Add(FLevelData());
		FString UUID = ResourceManagerPtr->GenerateUUID();
		VersusGames.Last().UUID = UUID;
		VersusGames.Last().LevelName = FName(*GameName);
		FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(FName(*UUID));
		if (ExistingSession == nullptr)
		{
			SetCurrentLevelData((VersusGames.Last()));
			CreateSession(FName(*UUID));
			return true;
		}
		else
		{
			SessionInterface->DestroySession(FName(*UUID));
			return false;
		}
	}
	return false;
}

bool UAcroGameInstance::CreateNewCreativeGame(const FString& GameName)
{
	if (SessionInterface.IsValid())
	{
		CreativeGames.Add(FLevelData());
		if (!ensure(ResourceManagerPtr->CreateNewGame(GameName, CreativeGames.Last()))) return false;
		FString UUID = CreativeGames.Last().UUID;
		FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(FName(*UUID));
		if (ExistingSession == nullptr)
		{
			SetCurrentLevelData((CreativeGames.Last()));
			CreateSession(FName(*UUID));
			return true;
		}
		else
		{
			SessionInterface->DestroySession(FName(*UUID));
			return false;
		}
	}
	return false;
}

bool UAcroGameInstance::LoadLevel(FLevelData LevelData)
{
	UE_LOG(LogTemp, Warning, TEXT("LoadLevel: %s"), *(LevelData.LevelName.ToString()));
	SetCurrentLevelData(LevelData);
	if (SessionInterface.IsValid())
	{
		for (const FOnlineSessionSearchResult & SearchResult : SessionSearch->SearchResults)
		{
			FString UUID;
			if (SearchResult.Session.SessionSettings.Get(TEXT("UUID"), UUID))
			{
				if (LevelData.UUID == UUID)
				{
					UE_LOG(LogTemp, Warning, TEXT("Found Session ID: %s"), *SearchResult.GetSessionIdStr());
					SessionInterface->JoinSession(0, FName(*UUID), SearchResult);
					return true;
				}
			}
		}

		FName UUIDFName = FName(*LevelData.UUID);
		UE_LOG(LogTemp, Warning, TEXT("Could not find Session, %s, Creating new"), *(LevelData.LevelName.ToString()));
		CreateSession(UUIDFName);
		return true;
	}
	return false;
}

bool UAcroGameInstance::DeleteSessions()
{
	for (int i = 0; i < VersusGames.Num(); i++)
	{
		SessionInterface->DestroySession(FName(*(VersusGames[i].UUID)));
	}
	for (int i = 0; i < CreativeGames.Num(); i++)
	{
		SessionInterface->DestroySession(FName(*(CreativeGames[i].UUID)));
	}
	return false;
}

void UAcroGameInstance::CreateSession(FName UUID)
{
	UE_LOG(LogTemp, Warning, TEXT("Found Session Interface"));
	FOnlineSessionSettings SessionSettings;
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		UE_LOG(LogTemp, Warning, TEXT("Is LAN Match"));
		SessionSettings.bIsLANMatch = true;
	}
	else // Steam
	{
		UE_LOG(LogTemp, Warning, TEXT("Is Steam Match"));
		SessionSettings.bIsLANMatch = false;
	}
	SessionSettings.NumPublicConnections = 4;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bUsesPresence = true;

	if (!CreativeGame)
	{
		FString LevelName = VersusGames.Last().LevelName.ToString();
		SessionSettings.Set(TEXT("GameMode"), FString("Versus"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionSettings.Set(TEXT("LevelName"), LevelName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	}
	SessionSettings.Set(TEXT("UUID"), UUID.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	FString TestUUID;
	if (SessionSettings.Get(TEXT("UUID"), TestUUID))
	{
		UE_LOG(LogTemp, Warning, TEXT("TEST: Found Session With UUID: "), *TestUUID);
	}
	SessionInterface->CreateSession(0, UUID, SessionSettings);
}

void UAcroGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Created Session: %s"), *SessionName.ToString());

		UWorld* World = GetWorld();
		if (!ensure(World != nullptr)) return;
		if (CreativeGame)
		{
			World->ServerTravel("/Game/Maps/GameLevel?game=/Script/Acro.AcroCreativeGameMode?listen");
		}
		else
		{
			World->ServerTravel("/Game/Maps/GameLevel?game=/Script/Acro.AcroVsGameMode?listen");
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not create Session: %s"), *SessionName.ToString());
	}
}

void UAcroGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Destroyed Session: %s"), *SessionName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not Destroy Session: %s"), *SessionName.ToString());
	}
}

void UAcroGameInstance::OnFindSessionsComplete(bool Success)
{
	bool NoResults = true;
	if (Success && SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Finished Find Sessions"));
		if (CreativeGame)
		{
			for (const FOnlineSessionSearchResult & SearchResult : SessionSearch->SearchResults)
			{
				FString UUID;
				if (SearchResult.Session.SessionSettings.Get(TEXT("UUID"), UUID))
				{
					
					UE_LOG(LogTemp, Warning, TEXT("Found Session With UUID: %s"), *UUID);
					for (int i = 0; i < CreativeGames.Num(); i++)
					{
						FString LevelUUID = CreativeGames[i].UUID;
						if (LevelUUID == UUID)
						{
							CreativeGames[i].SessionID = FString(*SearchResult.GetSessionIdStr());
							CreativeGames[i].ActivePlayers = SearchResult.Session.NumOpenPublicConnections;
						}
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Session has no UUID: "));
				}
				UE_LOG(LogTemp, Warning, TEXT("Found Session ID: %s"), *SearchResult.GetSessionIdStr());
			}
			for (int i = 0; i < CreativeGames.Num(); i++)
			{
				NoResults = false;
				LevelLoaded.Broadcast(CreativeGames[i]);
			}
		}
		else
		{
			for (const FOnlineSessionSearchResult & SearchResult : SessionSearch->SearchResults)
			{
				FString UUID;
				FString GameMode;
				if (SearchResult.Session.SessionSettings.Get(TEXT("UUID"), UUID) && SearchResult.Session.SessionSettings.Get(TEXT("GameMode"), GameMode))
				{
					if (GameMode.Equals(FString("Versus")))
					{
						FString LevelName;
						SearchResult.Session.SessionSettings.Get(TEXT("LevelName"), LevelName);
						NoResults = false;
						VersusGames.Add(FLevelData());
						VersusGames.Last().UUID = UUID;
						VersusGames.Last().LevelName = FName(*LevelName);
						VersusGames.Last().SessionID = FString(*SearchResult.GetSessionIdStr());
						VersusGames.Last().ActivePlayers = SearchResult.Session.NumOpenPublicConnections;
						LevelLoaded.Broadcast(VersusGames.Last());
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Session has no UUID: "));
				}
				UE_LOG(LogTemp, Warning, TEXT("Found Session ID: %s"), *SearchResult.GetSessionIdStr());
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not Find Sessions"));
	}
	if (NoResults)
	{
		NoLoadResults.Broadcast();
	}
}

void UAcroGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid()) return;

	FString Address;
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not get Connect String"));
	}

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

void UAcroGameInstance::ReturnToMainMenu()
{
	SessionInterface->DestroySession(FName(*CurrentLevelData.UUID));
	UGameplayStatics::OpenLevel((UObject*)this, FName("MainMenuLevel"));
}