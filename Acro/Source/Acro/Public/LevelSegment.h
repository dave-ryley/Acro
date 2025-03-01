// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AcroMesh.h"
#include "GameFramework/Actor.h"

class AAcroCreativePawn;

class ACRO_API FLevelSegment
{
public:
    FLevelSegment();
    ~FLevelSegment();

    void SaveLoadSegment(FArchive& Ar);
    void ConstructSegment(UWorld* World);
    
    void BeginGeneratingMesh(AActor* Actor, FVector Position);
    void ContinueGeneratingMesh(FVector Position);
    void CompleteGeneratingMesh();

    int16 MeshCount = 0;
    TArray<UAcroMesh*> Meshes;

private:
	UAcroMesh* CurrentMesh = nullptr;
};
