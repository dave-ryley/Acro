// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class AAcroPlayerState;

class ACRO_API IAcroPlayStateInterface
{
public:
    virtual void LeftMouseDown(FVector2D MousePosition) = 0;
    virtual void LeftMouseUp(FVector2D MousePosition) = 0;
    virtual void OnEnterState(AAcroPlayerState * PlayerState) = 0;
    virtual void OnExitState() = 0;
};
