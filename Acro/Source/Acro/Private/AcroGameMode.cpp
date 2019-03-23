// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "AcroGameMode.h"
#include "AcroGameInstance.h"
#include "AcroPlayerState.h"
#include "AcroPlayerController.h"

AAcroGameMode::AAcroGameMode()
{
	static ConstructorHelpers::FClassFinder<AAcroCharacter> PlayerPawnBPClass(TEXT("/Game/Blueprints/SideScrollerCharacter.SideScrollerCharacter_C"));
	if (PlayerPawnBPClass.Succeeded())
	{
		PlayerCharacterClass = PlayerPawnBPClass.Class;
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	PlayerStateClass = AAcroPlayerState::StaticClass();
	GameStateClass = AGameStateBase::StaticClass();
	PlayerControllerClass = AAcroPlayerController::StaticClass();
}

void AAcroGameMode::StartPlay()
{
	Super::StartPlay();
}

void AAcroGameMode::SaveMesh(UAcroMesh* Mesh)
{
	Meshes.Add(Mesh);
}

void AAcroGameMode::DeleteMesh(UAcroMesh* Mesh)
{
	Meshes.Remove(Mesh);
}
