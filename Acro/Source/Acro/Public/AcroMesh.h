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

	void ConstructLoadedMesh(UWorld* World);

	virtual bool IsSupportedForNetworking() const override { return true; }

	bool HasMeshActor();
	void SpawnMeshActor(UWorld* World);
	void BeginGeneratingMesh(FVector Position);
	void ContinueGeneratingMesh(FVector Position);
	bool EndGeneratingMesh();
	float ApplyDamage(float Amount);

	void DestroyMesh();

	TArray<FVector> CenterPositions;

	UPROPERTY(Replicated)
	AAcroMeshActor* MeshActor;

protected:
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

private:
	int8 Damage = 0;
};
