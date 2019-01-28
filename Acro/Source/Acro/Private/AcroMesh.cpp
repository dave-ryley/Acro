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

void UAcroMesh::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(UAcroMesh, CenterPositions);
	DOREPLIFETIME(UAcroMesh, CenterPositionsCount);
	DOREPLIFETIME(UAcroMesh, MeshActor);
}

void UAcroMesh::SaveLoadMesh(FArchive & Ar)
{
	Ar << CenterPositionsCount;
	if (CenterPositions.Num() < CenterPositionsCount)
	{
		CenterPositions.Reserve(CenterPositionsCount);
		for (int i = 0; i < CenterPositionsCount; i++)
		{
			CenterPositions.Add(FVector::ZeroVector);
		}
	}
	for (int i = 0; i < CenterPositionsCount; i++)
	{
		Ar << CenterPositions[i];
	}
}

void UAcroMesh::ConstructLoadedMesh(UWorld* World)
{
	if (CenterPositions.Num() > 1)
	{
		MeshActor = (AAcroMeshActor*)World->SpawnActor(AAcroMeshActor::StaticClass());
		MeshActor->GenerateMesh(&CenterPositions);
	}
	else
	{
		printf("Mesh has no Center Positions");
	}
}

void UAcroMesh::BeginGeneratingMesh(UWorld* World, FVector Position)
{
	if (MeshActor == nullptr)
	{
		MeshActor = (AAcroMeshActor*)World->SpawnActor(AAcroMeshActor::StaticClass());
	}
	else
	{
		CenterPositions.Empty();
	}
	CenterPositions.Add(Position);
	CenterPositionsCount = 1;
	MeshActor->GenerateMesh(&CenterPositions);
}

void UAcroMesh::ContinueGeneratingMesh(FVector Position)
{
	if (CenterPositionsCount < 2)
	{
		CenterPositions.Add(Position);
		CenterPositionsCount += 1;
	}
	else
	{
		FVector lengthVector = CenterPositions[CenterPositionsCount - 2] - CenterPositions[CenterPositionsCount - 1];
		float magnitude = lengthVector.Size();
		if (magnitude > VECTOR_LENGTH_THRESHOLD)
		{
			CenterPositions.Add(Position);
			CenterPositionsCount += 1;
		}
		else
		{
			CenterPositions[CenterPositionsCount - 1] = Position;
		}
	}
	MeshActor->GenerateMesh(&CenterPositions);
}

void UAcroMesh::EndGeneratingMesh()
{
}
