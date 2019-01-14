// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AcroMesh.h"

class AAcroCreativePawn;

class ACRO_API FLevelSegment
{
public:
    FLevelSegment();
    ~FLevelSegment();

    void SaveLoadSegment(FArchive& Ar);
    void ConstructSegment(UWorld* World);
    
    void BeginGeneratingMesh(AAcroCreativePawn* Pawn, FVector Position);
    void ContinueGeneratingMesh(FVector Position);
    void CompleteGeneratingMesh();

    int16 MeshCount = 0;
    TArray<FAcroMesh> Meshes;

private:
    FAcroMesh * CurrentMesh = nullptr;
};
