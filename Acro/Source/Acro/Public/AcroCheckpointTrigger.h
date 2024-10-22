// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "AcroCheckpointTrigger.generated.h"

UCLASS()
class ACRO_API AAcroCheckpointTrigger : public ATriggerBox
{
    GENERATED_BODY()

public:

    AAcroCheckpointTrigger(const FObjectInitializer& ObjectInitializer);

    void SetCheckPointType(bool Validator);
    
    UFUNCTION()
    void OnEnterLevelComplete(class AActor* OverlappedActor, class AActor* OtherActor);

    UFUNCTION()
    void OnEnterLevelValidation(class AActor* OverlappedActor, class AActor* OtherActor);

};
