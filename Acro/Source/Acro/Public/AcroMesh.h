// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "AcroMeshActor.h"
#include "CoreMinimal.h"
#include "AcroMesh.generated.h"

UCLASS()
class ACRO_API UAcroMesh : public UObject
{
	GENERATED_BODY()

public:
	UAcroMesh();
	~UAcroMesh();

	void SaveLoadMesh(FArchive& Ar);
	void ConstructLoadedMesh(UWorld* World);

	void BeginGeneratingMesh(UWorld* World, FVector Position);
	void ContinueGeneratingMesh(FVector Position);
	void EndGeneratingMesh();

protected:
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

private:

	UPROPERTY(Replicated)
	TArray<FVector> CenterPositions;
	UPROPERTY(Replicated)
	int16 CenterPositionsCount = 0;
	UPROPERTY(Replicated)
	AAcroMeshActor* MeshActor;
};
