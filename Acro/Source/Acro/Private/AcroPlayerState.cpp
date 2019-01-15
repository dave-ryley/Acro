// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "../Public/AcroPlayerState.h"
#include "../Public/AcroDefinitions.h"

void AAcroPlayerState::ChangeState(IAcroPlayStateInterface * NewState)
{
    print("Changing State");
    if (CurrentState != NewState)
    {
        print("CurrentState != NewState");
        if (CurrentState != nullptr)
        {
            CurrentState->OnExitState();
        }
        CurrentState = NewState;
        CurrentState->OnEnterState(this);
    }
}
