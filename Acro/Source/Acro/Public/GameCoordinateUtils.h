// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

class ACRO_API GameCoordinateUtils
{
public:
	static FVector2D ScreenToGameCoordinates(APlayerController* PlayerController, FVector2D ScreenCoordinates);
	static FVector2D WorldToGameCoordinates(FVector WorldCoordinates);
	static FVector2D GameToScreenCoordinates(APlayerController* PlayerController, FVector2D GameCoordinates);
	static FVector GameToWorldCoordinates(FVector2D GameCoordinates);
	static FVector ScreenToWorldCoordinates(APlayerController* PlayerController, FVector2D ScreenCoordinates);
};
