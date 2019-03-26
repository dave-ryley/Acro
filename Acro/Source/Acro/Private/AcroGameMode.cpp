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
	GameStateClass = AGameState::StaticClass();
	PlayerControllerClass = AAcroPlayerController::StaticClass();
	ProjectilePool = NewObject<UProjectilePool>();
	ProjectilePool->SetupBP(TEXT("/Game/Blueprints/Snowball"));
}

void AAcroGameMode::InitGame(const FString & MapName, const FString & Options, FString & ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	ProjectilePool->Initialize(GetWorld(), 64, 32);
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

void AAcroGameMode::ThrowProjectile(FVector2D Position, FVector2D Direction)
{
	AProjectile *Projectile = ProjectilePool->Acquire(GetWorld());
	Projectile->Spawn(Position, Direction);
}

void AAcroGameMode::PauseGame(bool Value)
{
	UGameplayStatics::SetGamePaused((UObject*)this, Value);
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (APlayerController* PlayerController = Iterator->Get())
		{
			AAcroCharacter* Char = Cast<AAcroCharacter>(PlayerController->GetPawn());
			if (Value)
			{
				Char->OnPauseGame();
			}
			else
			{
				Char->OnUnpauseGame();
			}
		}
	}
}