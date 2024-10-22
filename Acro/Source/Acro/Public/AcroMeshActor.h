// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RuntimeMeshComponent.h"
#include "Sound/SoundCue.h"
#include "AcroMeshActor.generated.h"


typedef TFunction<void(
	const FVector& Position,
	const FVector& Normal,
	const FRuntimeMeshTangent& Tangent,
	const FVector2D& UV0
)> FVerticesBuilderFunction;

typedef TFunction<void(int32 Index1, int32 Index2, int32 Index3)> FTriangleBuilderFunction;

class UAcroMesh;

UCLASS()
class ACRO_API AAcroMeshActor : public AActor
{
	GENERATED_BODY()

public:
	AAcroMeshActor(const FObjectInitializer& ObjectInitializer);

	class URuntimeMeshComponent* GetRuntimeMeshComponent() const { return RuntimeMeshComponent; }

	void SetParent(UAcroMesh* ParentAcroMesh);
	void GenerateMesh();
	void SetupMesh();
	void BeginGeneratingMesh(FVector Position);
	void ContinueGeneratingMesh(FVector Position);
	bool EndGeneratingMesh();

	virtual void Tick(float DeltaSeconds) override;

	void Hit(FVector Position, FVector Direction);

	UFUNCTION(NetMulticast, Reliable)
	void DestroyVisuals(FVector Position, FVector Direction);
	void DestroyVisuals_Implementation(FVector Position, FVector Direction);

	UFUNCTION(NetMulticast, Reliable)
	void DamageVisuals(float DamageAmount);
	void DamageVisuals_Implementation(float DamageAmount);

	UPROPERTY(Replicated)
	TArray<FVector> CenterPositions;


protected:
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

private:
	bool bIsSetup;
	UPROPERTY(Replicated)
	class URuntimeMeshComponent* RuntimeMeshComponent;
	class UMaterial* MeshMaterial;
	class UMaterialInstanceDynamic* MeshMaterialInstance;
	UAcroMesh* Parent;

	UParticleSystem* ParticleInstance;
	UParticleSystem* DamageParticleInstance;
	USoundCue* DestroySound;
	USoundCue* DamageSound;
};
