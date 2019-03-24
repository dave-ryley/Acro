// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "AcroVsGameMode.h"


void AAcroVsGameMode::InitGame(const FString & MapName, const FString & Options, FString & ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	UE_LOG(LogTemp, Warning, TEXT("InitGame Vs."));
}

void AAcroVsGameMode::StartPlay()
{
	AAcroGameMode::StartPlay();
	UE_LOG(LogTemp, Warning, TEXT("StartPlay Vs."));
}