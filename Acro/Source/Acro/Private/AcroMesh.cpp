// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "AcroMesh.h"
#include "../Public/AcroDefinitions.h"
#include "UnrealNetwork.h"

UAcroMesh::UAcroMesh()
{
}

UAcroMesh::~UAcroMesh()
{
}

// Called from GameMode when clearing all meshes.
void UAcroMesh::DestroyMesh()
{
	if (MeshActor != nullptr)
	{
		MeshActor->DestroyVisuals(FVector::ZeroVector, FVector::UpVector);
		MeshActor->Destroy();
	}
}

float UAcroMesh::ApplyDamage(float Amount)
{
	Damage += Amount;
	return Damage;
}

void UAcroMesh::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(UAcroMesh, MeshActor);
}

void UAcroMesh::ConstructLoadedMesh(UWorld* World)
{
	if (CenterPositions.Num() > 1)
	{
		MeshActor = (AAcroMeshActor*)World->SpawnActor(AAcroMeshActor::StaticClass());
		MeshActor->SetParent(this);
		//MeshActor->GenerateMesh(&CenterPositions);
		MeshActor->BeginGeneratingMesh(CenterPositions[0]);
		for (int i = 1; i < CenterPositions.Num(); i++)
		{
			MeshActor->ContinueGeneratingMesh(CenterPositions[i]);
		}
	}
}

bool UAcroMesh::HasMeshActor()
{
	return (MeshActor != nullptr);
}

void UAcroMesh::SpawnMeshActor(UWorld* World)
{
	MeshActor = (AAcroMeshActor*)World->SpawnActor(AAcroMeshActor::StaticClass());
	MeshActor->SetParent(this);
}

void UAcroMesh::BeginGeneratingMesh(FVector Position)
{
	MeshActor->BeginGeneratingMesh(Position);
}

void UAcroMesh::ContinueGeneratingMesh(FVector Position)
{
	MeshActor->ContinueGeneratingMesh(Position);
}

bool UAcroMesh::EndGeneratingMesh()
{
	bool Keep = MeshActor->EndGeneratingMesh();
	if (!Keep)
	{
		DestroyMesh();
	}
	return Keep;
}
