// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AcroPlayStateInterface.h"

class AAcroPlayerState;

class ACRO_API FAcroPlayStateCreate : public IAcroPlayStateInterface
{
public:

    void LeftMouseDown(FVector2D MousePosition) override;
    void LeftMouseUp(FVector2D MousePosition) override;
    void OnEnterState(AAcroPlayerState* PlayerState) override;
    void OnExitState() override;

    FAcroPlayStateCreate();
    virtual ~FAcroPlayStateCreate();
};
