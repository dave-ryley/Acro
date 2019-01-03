// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "AcroGameMode.h"
#include "AcroCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAcroGameMode::AAcroGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/SideScrollerCPP/Blueprints/SideScrollerCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
