// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include <vector>
#include "MeshPlatform.h"
#include "CoreMinimal.h"

class ACRO_API FAcroMesh
{
public:
    FAcroMesh();
    ~FAcroMesh();

    void SaveLoadMesh(FArchive& Ar);
    void ConstructLoadedMesh(UWorld* World);

    void BeginGeneratingMesh(UWorld* World, FVector Position);
    void ContinueGeneratingMesh(FVector Position);
    void EndGeneratingMesh(FVector Position);

private:

    TArray<FVector> CenterPositions;
    int16 CenterPositionsCount = 0;
    AMeshPlatform* Platform;
};
