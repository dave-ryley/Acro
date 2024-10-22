// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "AcroVsGameMode.h"

void AAcroVsGameMode::InitGame(const FString & MapName, const FString & Options, FString & ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

void AAcroVsGameMode::StartPlay()
{
	Super::StartPlay();
	StartTimer = 0.f;
}

void AAcroVsGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (NumPlayers > 1)
	{
		StartTimer += DeltaSeconds;
	}
}

void AAcroVsGameMode::StartMatch()
{
	Super::StartMatch();
	UWorld* World = GetWorld();
	for (int i = Meshes.Num() - 1; i >= 0; i--)
	{
		if (Meshes[i] != nullptr)
		{
			Meshes[i]->DestroyMesh();
		}
		Meshes.RemoveAt(i);
	}
	// Place Characters
	int PlayerIndex = 0;
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (APlayerController* PlayerController = Iterator->Get())
		{
			FVector Location = FVector(2000.f, 0.f, 100.f);
			Location = Location.RotateAngleAxis(90.f*PlayerIndex, FVector::UpVector);
			AAcroCharacter* Character = Cast<AAcroCharacter>(PlayerController->GetPawn());
			Character->StartMatch(Location);
			PlayerIndex++;
		}
	}
}

bool AAcroVsGameMode::ReadyToStartMatch_Implementation()
{
	return (StartTimer > 2);
}

void AAcroVsGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	RestartPlayer(NewPlayer);
}

void AAcroVsGameMode::LoseGame(ACharacter* Character)
{
	AAcroCharacter* LosingCharacter = Cast<AAcroCharacter>(Character);
	LosingCharacter->LoseGame();
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (APlayerController* PlayerController = Iterator->Get())
		{
			AAcroCharacter* Char = Cast<AAcroCharacter>(PlayerController->GetPawn());
			if (Char != LosingCharacter)
			{
				Char->WinGame();
			}
		}
	}
	EndMatch();
}