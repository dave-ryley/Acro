// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AcroPlayStateInterface.h"

class AAcroPlayerState;

class ACRO_API FAcroPlayStatePlay : public IAcroPlayStateInterface
{
public:

    virtual void LeftMouseDown(FVector2D MousePosition) override;
    virtual void LeftMouseUp(FVector2D MousePosition) override;
    virtual void OnEnterState(AAcroPlayerState* PlayerState) override;
    virtual void OnExitState() override;

    FAcroPlayStatePlay();
    virtual ~FAcroPlayStatePlay();
};
