// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include <vector>
#include "CoreMinimal.h"
#include "RuntimeMeshComponent.h"
#include "RuntimeMeshActor.h"
#include "MeshPlatform.generated.h"

typedef TFunction<void(const FVector& Position, const FVector& Normal, const FRuntimeMeshTangent& Tangent, const FVector2D& UV0)> FVerticesBuilderFunction;
typedef TFunction<void(int32 Index)> FTrianglesBuilderFunction;

UCLASS()
class ACRO_API AMeshPlatform : public ARuntimeMeshActor
{
    GENERATED_BODY()
    
public:
    // Sets default values for this actor's properties
    AMeshPlatform();

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FVector BoxSize;

    virtual void BeginGeneratingMesh(FVector startingPos);
    virtual void ContinueGeneratingMesh(FVector pos);
    virtual void EndGeneratingMesh(FVector endPos);

    void AddCenterPosition(FVector);

protected:
    virtual void GenerateMeshes_Implementation() override; // Called *_Implementation because GenerateMeshes() is blueprintNative
    void ContinueGeneratingMeshes();

private:

    std::vector<FVector> _centerPositions;
    int16 _centerPositionsCount = 0;
    void CreateCustomMesh(FRuntimeMeshAccessor&);
    void CreateBoxMesh(FVector, FVerticesBuilderFunction, FTrianglesBuilderFunction);
    void CreatePlatformMesh(float radius, int radVerts, float offset, FVerticesBuilderFunction, FRuntimeMeshAccessor&);
    void ConvertQuadToTriangles(TFunction<void(int32 Index)>, int32, int32, int32, int32);
};
