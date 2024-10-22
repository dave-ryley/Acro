// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "../Public/AcroPlayerState.h"
#include "../Public/AcroCharacter.h"
#include "../Public/AcroDefinitions.h"

void AAcroPlayerState::ChangeState(IAcroPlayStateInterface * NewState)
{
    if (CurrentState != NewState)
    {
        if (CurrentState != nullptr)
        {
            CurrentState->OnExitState();
        }
        CurrentState = NewState;
        CurrentState->OnEnterState(this);
    }
}