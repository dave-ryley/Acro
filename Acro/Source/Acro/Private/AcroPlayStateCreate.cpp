// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "../Public/AcroPlayStateCreate.h"
#include "../Public/AcroCheckpointTrigger.h"
#include "../Public/AcroDefinitions.h"
#include "../Public/AcroCreativeGameMode.h"
#include "../Public/AcroPlayerState.h"

void FAcroPlayStateCreate::LeftMouseDown(FVector2D MousePosition)
{
}

void FAcroPlayStateCreate::LeftMouseUp(FVector2D MousePosition)
{
}

void FAcroPlayStateCreate::OnEnterState(AAcroPlayerState* PlayerState)
{
    // TODO: Have GameMode hold on to trigger boxes.

    AAcroCheckpointTrigger* CheckpointBox = PlayerState->GetWorld()->SpawnActor<AAcroCheckpointTrigger>(AAcroCheckpointTrigger::StaticClass());
    AAcroCreativeGameMode* GameMode = Cast<AAcroCreativeGameMode>(PlayerState->GetWorld()->GetAuthGameMode());
    CheckpointBox->SetCheckPointType(true);

    FVector Position = FVector(LEVEL_RADIUS, 0.0, 50.0 + LEVEL_HEIGHT * (GameMode->LevelSegments.Num() + 1));
    float rotationAmount = ((GameMode->LevelSegments.Num() + 1) * LEVEL_WIDTH) / LEVEL_CIRCUMFERENCE * 360.f;
    Position = Position.RotateAngleAxis(rotationAmount, FVector::UpVector);

    CheckpointBox->SetActorLocation(Position);
}

void FAcroPlayStateCreate::OnExitState()
{
}

FAcroPlayStateCreate::FAcroPlayStateCreate()
{
}

FAcroPlayStateCreate::~FAcroPlayStateCreate()
{
}
