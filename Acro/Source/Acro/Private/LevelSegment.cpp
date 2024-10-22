// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "LevelSegment.h"
#include "../Public/AcroDefinitions.h"

FLevelSegment::FLevelSegment()
{
}

FLevelSegment::~FLevelSegment()
{
}

void FLevelSegment::BeginGeneratingMesh(AActor* Actor, FVector Position)
{
    MeshCount++;
    Meshes.Add(NewObject<UAcroMesh>());
    CurrentMesh = Meshes.Last();
    //UWorld* World = Actor->GetWorld();
    //CurrentMesh->BeginGeneratingMesh(World, Position);
}

void FLevelSegment::ContinueGeneratingMesh(FVector Position)
{
    if (CurrentMesh != nullptr)
    {
        CurrentMesh->ContinueGeneratingMesh(Position);
    }
}

void FLevelSegment::CompleteGeneratingMesh()
{
    CurrentMesh = nullptr;
}

void FLevelSegment::SaveLoadSegment(FArchive& Ar)
{
    // Ar << MeshCount;
    // for (int i = 0; i < MeshCount; i++)
    // {
    //     if (Meshes.Num() <= i) // Should only evaluate to false when loading.
    //     {
    //         Meshes.Add(NewObject<UAcroMesh>());
    //     }
    //     Meshes[i]->SaveLoadMesh(Ar);
    // }
}

void FLevelSegment::ConstructSegment(UWorld* World)
{
    for (int i = 0; i < MeshCount; i++)
    {
        Meshes[i]->ConstructLoadedMesh(World);
    }
}
