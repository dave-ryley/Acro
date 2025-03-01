// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AcroPlayStateInterface.h"
#include "AcroPlayStatePlay.h"
#include "AcroPlayStateCreate.h"
#include "AcroPlayerState.generated.h"

UCLASS()
class ACRO_API AAcroPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    FAcroPlayStatePlay PlayState;
    FAcroPlayStateCreate CreateState;
    IAcroPlayStateInterface* CurrentState = &PlayState;

    void ChangeState(IAcroPlayStateInterface * NewState);
};
