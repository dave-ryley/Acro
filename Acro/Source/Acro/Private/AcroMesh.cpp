// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "AcroMesh.h"
#include "../Public/AcroDefinitions.h"

FAcroMesh::FAcroMesh()
{
}

FAcroMesh::~FAcroMesh()
{
}

void FAcroMesh::SaveLoadMesh(FArchive & Ar)
{
    Ar << CenterPositionsCount;
    if (CenterPositions.Num() < CenterPositionsCount)
    {
        CenterPositions.Reserve(CenterPositionsCount);
        for (int i = 0; i < CenterPositionsCount; i++)
        {
            CenterPositions.Add(FVector::ZeroVector);
        }
    }
    for (int i = 0; i < CenterPositionsCount; i++)
    {
        Ar << CenterPositions[i];
    }
}

void FAcroMesh::ConstructLoadedMesh(UWorld* World)
{
    Platform = (AMeshPlatform*)World->SpawnActor(AMeshPlatform::StaticClass());
    for (int i = 0; i < CenterPositions.Num(); i++)
    {
        Platform->AddCenterPosition(CenterPositions[i]);
    }
    Platform->ContinueGeneratingMesh(CenterPositions[CenterPositions.Num()-1]);
}

void FAcroMesh::BeginGeneratingMesh(UWorld* World, FVector Position)
{
    Platform = (AMeshPlatform*)World->SpawnActor(AMeshPlatform::StaticClass());
    CenterPositions.Add(Position);
    CenterPositionsCount = 1;
    Platform->BeginGeneratingMesh(Position);
}

void FAcroMesh::ContinueGeneratingMesh(FVector Position)
{
    if (CenterPositionsCount < 2)
    {
        CenterPositions.Add(Position);
        CenterPositionsCount += 1;
    }
    else
    {
        FVector lengthVector = CenterPositions[CenterPositionsCount - 2] - CenterPositions[CenterPositionsCount - 1];
        float magnitude = lengthVector.Size();
        if (magnitude > VECTOR_LENGTH_THRESHOLD)
        {
            CenterPositions.Add(Position);
            CenterPositionsCount += 1;
        }
        else
        {
            CenterPositions[CenterPositionsCount - 1] = Position;
        }
    }
    Platform->ContinueGeneratingMesh(Position);
}

void FAcroMesh::EndGeneratingMesh(FVector Position)
{
}
