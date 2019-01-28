// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "AcroMeshActor.h"
#include "RuntimeMeshComponent.h"
#include "../Public/AcroMeshActor.h"

AAcroMeshActor::AAcroMeshActor(const FObjectInitializer& ObjectInitializer) 
{
	RuntimeMeshComponent = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("RuntimeMeshComponent0"));
	RuntimeMeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	RuntimeMeshComponent->Mobility = EComponentMobility::Static;

	RuntimeMeshComponent->SetGenerateOverlapEvents(false);
	RootComponent = RuntimeMeshComponent;
	bReplicates = true;
	//bReplicateMovement = true; // May invoke later if we want the meshes to move.
}

void ConvertQuadToTriangles(TFunction<void(int32 Index)> TrianglesBuilder, 
	int32 Vert0, int32 Vert1, int32 Vert2, int32 Vert3)
{
	TrianglesBuilder(Vert0);
	TrianglesBuilder(Vert1);
	TrianglesBuilder(Vert3);

	TrianglesBuilder(Vert1);
	TrianglesBuilder(Vert2);
	TrianglesBuilder(Vert3);
}

void AAcroMeshActor::GenerateMesh(TArray<FVector>* Positions)
{
	FRuntimeMeshDataPtr Data = GetRuntimeMeshComponent()->GetOrCreateRuntimeMesh()->GetRuntimeMeshData();
	// TODO: Change to UpdateMeshSection()
	Data->CreateMeshSection(0, false, false, 1, false, true, EUpdateFrequency::Average);

	TUniquePtr<FRuntimeMeshScopedUpdater> Section = Data->BeginSectionUpdate(0);
	FRuntimeMeshAccessor& MeshBuilder = *Section.Get();

	int radVerts = 8;
	float radius = 35.f;
	float rotationOffset = 45.f;
	int numVerts = 24;
	int numTris = 36;

	MeshBuilder.EmptyVertices(numVerts);
	MeshBuilder.EmptyIndices(numTris);

	FVerticesBuilderFunction VerticesBuilder = [&](const FVector& Position, 
		const FVector& Normal, const FRuntimeMeshTangent& Tangent, const FVector2D& UV0)
	{
		int32 NewVertex = MeshBuilder.AddVertex(Position);
		MeshBuilder.SetNormalTangent(NewVertex, Normal, Tangent);
		MeshBuilder.SetUV(NewVertex, UV0);
	};

	FTrianglesBuilderFunction TrianglesBuilder = [&](int32 Index)
	{
		MeshBuilder.AddIndex(Index);
	};

	TArray<FVector> Verts = TArray<FVector>();
	for (int i = 0; i < Positions->Num(); i++)
	{
		FVector Center = (*Positions)[i];
		FVector Normal = FVector(0.f, 0.f, 0.f);
		if (i > 0)                                  Normal += Center - (*Positions)[i - 1];
		if (i < Positions->Num() - 1)              Normal += (*Positions)[i + 1] - Center;

		float Magnitude = Normal.Size();
		if (Magnitude > 0)
		{
			Normal /= Magnitude;
		}
		else
		{
			Normal = FVector(0.f, 0.f, 1.f);
		}
		FVector Tangent = FVector(Center.X, Center.Y, 0.f); // Flattening the center vector into 2D to get the tangent
		Magnitude = Tangent.Size();
		if (Magnitude > 0)
		{
			Tangent /= Magnitude;
		}
		FVector Vert = Tangent * radius;

		for (int v = 0; v < radVerts; v++)
		{
			Verts.Add(
				Vert.RotateAngleAxis(
				((float)v / (float)(radVerts + 1))*360.f + rotationOffset,
					Normal)
				+ Center);
		}
	}

	FVector Normal;
	FRuntimeMeshTangent Tangent;

	Normal = FVector::CrossProduct(Verts[2] - Verts[0], Verts[1] - Verts[0]);
	Normal /= Normal.Size();
	Tangent.TangentX = (Verts[1] - Verts[0]);
	Tangent.TangentX.Normalize();
	for (int i = 0; i < radVerts; i++)
	{
		VerticesBuilder(Verts[i], Normal, Tangent, FVector2D(0.0f, 0.0f));
		if (i > 1)
		{
			TrianglesBuilder(0);
			TrianglesBuilder(i - 1);
			TrianglesBuilder(i);
		}
	}
	int triangleVertOffset = radVerts;

	for (int i = 1; i < Positions->Num(); i++)
	{
		FVector center = (*Positions)[i];
		FVector normal = FVector(0.f, 0.f, 0.f);
		if (i > 0)                                      normal += center - (*Positions)[i - 1];
		if (i < Positions->Num() - 1)              normal += (*Positions)[i + 1] - center;

		normal /= normal.Size(); // Dividing by magnitude to normalize
		FVector tangent = FVector(center.X, center.Y, 0.f); // Flattening the center vector into 2D to get the tangent
		tangent /= tangent.Size(); // Dividing by magnitude to normalize
		FVector vert = tangent * 25.f;

		int indexOffset = i * radVerts;

		for (int v = 1; v < radVerts; v++)
		{
			FVector vert1 = Verts[v - 1 + (i - 1)*radVerts];
			FVector vert2 = Verts[v - 1 + i * radVerts];
			FVector vert3 = Verts[v + i * radVerts];
			FVector vert4 = Verts[v + (i - 1)*radVerts];

			Normal = FVector::CrossProduct(vert4 - vert1, vert2 - vert1);
			Normal /= Normal.Size();
			Tangent.TangentX = vert2 - vert1;
			Tangent.TangentX.Normalize();
			VerticesBuilder(vert1, Normal, Tangent, FVector2D((float)(v - 1) / (float)(radVerts), (i - 1) / (float)(Positions->Num() - 1)));
			VerticesBuilder(vert2, Normal, Tangent, FVector2D((float)(v - 1) / (float)(radVerts), (float)(i) / (float)(Positions->Num() - 1)));
			VerticesBuilder(vert3, Normal, Tangent, FVector2D((float)(v) / (float)(radVerts), (float)(i) / (float)(Positions->Num() - 1)));
			VerticesBuilder(vert4, Normal, Tangent, FVector2D((float)(v) / (float)(radVerts), (float)(i - 1) / (float)(Positions->Num() - 1)));
			ConvertQuadToTriangles(TrianglesBuilder, 0 + triangleVertOffset, 1 + triangleVertOffset, 2 + triangleVertOffset, 3 + triangleVertOffset);
			triangleVertOffset += 4;
		}
		FVector vert1 = Verts[radVerts - 1 + (i - 1)*radVerts];
		FVector vert2 = Verts[radVerts - 1 + i * radVerts];
		FVector vert3 = Verts[i * radVerts];
		FVector vert4 = Verts[(i - 1)*radVerts];

		Normal = FVector::CrossProduct(vert4 - vert1, vert2 - vert1);
		Normal /= Normal.Size();
		Tangent.TangentX = vert2 - vert1;
		Tangent.TangentX.Normalize();
		VerticesBuilder(vert1, Normal, Tangent, FVector2D((float)(radVerts - 1) / (float)(radVerts), (i - 1) / (float)(Positions->Num() - 1)));
		VerticesBuilder(vert2, Normal, Tangent, FVector2D((float)(radVerts - 1) / (float)(radVerts), (float)(i) / (float)(Positions->Num() - 1)));
		VerticesBuilder(vert3, Normal, Tangent, FVector2D((float)0.f, (float)(i) / (float)(Positions->Num() - 1)));
		VerticesBuilder(vert4, Normal, Tangent, FVector2D((float)0.f, (float)(i - 1) / (float)(Positions->Num() - 1)));
		ConvertQuadToTriangles(TrianglesBuilder, 0 + triangleVertOffset, 1 + triangleVertOffset, 2 + triangleVertOffset, 3 + triangleVertOffset);
		triangleVertOffset += 4;
	}

	int start = (radVerts * (Positions->Num() - 1));

	Normal = FVector::CrossProduct(Verts[start + 1] - Verts[start], Verts[start + 2] - Verts[start]);
	Normal /= Normal.Size();
	Tangent.TangentX = (Verts[start + 2] - Verts[start]);
	Tangent.TangentX.Normalize();
	for (int i = 0; i < radVerts; i++)
	{
		VerticesBuilder(Verts[start + i], Normal, Tangent, FVector2D(0.0f, 0.0f));
		if (i > 1)
		{
			TrianglesBuilder(triangleVertOffset + 0);
			TrianglesBuilder(triangleVertOffset + i);
			TrianglesBuilder(triangleVertOffset + i - 1);
		}
	}

	Section->Commit();
}
