// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AcroMainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACRO_API UAcroMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void OnLoadMenu();
	
	
};
