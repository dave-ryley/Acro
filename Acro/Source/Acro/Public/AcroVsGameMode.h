// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AcroGameMode.h"
#include "AcroVsGameMode.generated.h"

UCLASS()
class ACRO_API AAcroVsGameMode : public AAcroGameMode
{
	GENERATED_BODY()
	
public:
	// Loads the Level Segments from disk
	virtual void InitGame(const FString & MapName, const FString & Options, FString & ErrorMessage) override;
	virtual void StartPlay() override;
	
	
};
