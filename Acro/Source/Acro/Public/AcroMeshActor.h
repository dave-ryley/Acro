// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RuntimeMeshComponent.h"
#include "AcroMeshActor.generated.h"

typedef TFunction<void(
	const FVector& Position, 
	const FVector& Normal, 
	const FRuntimeMeshTangent& Tangent, 
	const FVector2D& UV0
)> FVerticesBuilderFunction;

typedef TFunction<void(int32 Index)> FTrianglesBuilderFunction;

UCLASS()
class ACRO_API AAcroMeshActor : public AActor
{
	GENERATED_BODY()
	
public:
	AAcroMeshActor(const FObjectInitializer& ObjectInitializer);

	class URuntimeMeshComponent* GetRuntimeMeshComponent() const { return RuntimeMeshComponent; }
	
	void GenerateMesh(TArray<FVector>* Positions);

private:
	UPROPERTY(Replicated)
	class URuntimeMeshComponent* RuntimeMeshComponent;
	
};
