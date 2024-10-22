// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "AcroMeshActor.h"
#include "AcroMesh.h"
#include "AcroGameMode.h"
#include "RuntimeMeshComponent.h"
#include "AcroMeshActor.h"
#include "AcroDefinitions.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Particles/ParticleSystemComponent.h"
#include "UnrealNetwork.h"

const float MaxDamage = 45.f;

AAcroMeshActor::AAcroMeshActor(const FObjectInitializer& ObjectInitializer)
{

	RuntimeMeshComponent = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("RuntimeMeshComponent0"));
	RuntimeMeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	RuntimeMeshComponent->Mobility = EComponentMobility::Static;

	RuntimeMeshComponent->SetGenerateOverlapEvents(false);
	RootComponent = RuntimeMeshComponent;
	bReplicates = true;
	bIsSetup = false;

	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);

	static ConstructorHelpers::FObjectFinder<USoundCue> DestroySFX_BP(TEXT("/Game/SFX/Shatter.Shatter"));
	if (DestroySFX_BP.Object != NULL)
	{
		DestroySound = (USoundCue*)DestroySFX_BP.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundCue> DamageSFX_BP(TEXT("/Game/SFX/Crack.Crack"));
	if (DamageSFX_BP.Object != NULL)
	{
		DamageSound = (USoundCue*)DamageSFX_BP.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Materials/Ice_M.Ice_M'"));
	if (Material.Object != NULL)
	{
		MeshMaterial = (UMaterial*)Material.Object;
	}
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleBP(TEXT("/Game/FX/IceShatter_P.IceShatter_P"));
	if (ParticleBP.Succeeded())
	{
		ParticleInstance = ParticleBP.Object;
	}
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleDamageBP(TEXT("/Game/FX/IceDamage_P.IceDamage_P"));
	if (ParticleDamageBP.Succeeded())
	{
		DamageParticleInstance = ParticleDamageBP.Object;
	}
	//bReplicateMovement = true; // May invoke later if we want the meshes to move.
}

void AAcroMeshActor::SetParent(UAcroMesh* ParentAcroMesh)
{
	Parent = ParentAcroMesh;
}

void AAcroMeshActor::Tick(float DeltaSeconds)
{
	if(!HasAuthority())
	{
		if (bIsSetup)
		{
			GenerateMesh();
		}
		else
		{
			SetupMesh();
		}
	}
}

void AAcroMeshActor::BeginGeneratingMesh(FVector Position)
{
	CenterPositions.Empty();
	CenterPositions.Add(Position);
	SetupMesh();
}

void AAcroMeshActor::ContinueGeneratingMesh(FVector Position)
{
	if (CenterPositions.Num() < 2)
	{
		CenterPositions.Add(Position);
	}
	else
	{
		FVector lengthVector = Position - CenterPositions[CenterPositions.Num() - 2];
		float magnitude = lengthVector.Size();
		if (magnitude > VECTOR_CUTOFF_THRESHOLD)
		{
			FVector NewPosition = FVector(lengthVector);
			NewPosition.Normalize();
			NewPosition = (NewPosition * VECTOR_LENGTH_THRESHOLD);
			NewPosition += CenterPositions[CenterPositions.Num() - 2];
			CenterPositions[CenterPositions.Num() - 1] = NewPosition;
			CenterPositions.Add(Position);
		}
		else
		{
			CenterPositions[CenterPositions.Num() - 1] = Position;
		}
	}
	GenerateMesh();
}

bool AAcroMeshActor::EndGeneratingMesh()
{
	if (CenterPositions.Num() > 2)
	{
		return true;
	}
	else
	{
		return ((CenterPositions[0] - CenterPositions[1]).Size() > 1);
	}
}

void ConvertQuadToTriangles(TFunction<void(int32, int32, int32)> TriangleBuilder,
	int32 Vert0, int32 Vert1, int32 Vert2, int32 Vert3)
{
	TriangleBuilder(Vert0, Vert1, Vert3);
	TriangleBuilder(Vert1, Vert2, Vert3);
}

void AAcroMeshActor::SetupMesh()
{
	FRuntimeMeshDataPtr Data = GetRuntimeMeshComponent()->GetOrCreateRuntimeMesh()->GetRuntimeMeshData();
	Data->CreateMeshSection(0, false, false, 1, false, true, EUpdateFrequency::Average);
	MeshMaterialInstance = UMaterialInstanceDynamic::Create(MeshMaterial, this);
	GetRuntimeMeshComponent()->SetSectionMaterial(0, MeshMaterialInstance);
	bIsSetup = true;
}

void AAcroMeshActor::GenerateMesh()
{
	FRuntimeMeshDataPtr Data = GetRuntimeMeshComponent()->GetOrCreateRuntimeMesh()->GetRuntimeMeshData();
	TUniquePtr<FRuntimeMeshScopedUpdater> Section = Data->BeginSectionUpdate(0);
	FRuntimeMeshAccessor& MeshBuilder = *Section.Get();

	int capDepth = 30;
	int capSegments = 5;
	int radVerts = 16;
	float radius = 35.f;
	float rotationOffset = 0.f;
	int numVerts = 24; // Expected number of Verts.. should determine this for optimized array resizing.
	int numTris = 36; // Expected number of Indices.. should determine this for optimized array resizing.

	MeshBuilder.EmptyVertices(numVerts);
	MeshBuilder.EmptyIndices(numTris);
	TArray<int16> Triangles;

	FVerticesBuilderFunction VerticesBuilder = [&](const FVector& Position,
		const FVector& Normal, const FRuntimeMeshTangent& Tangent, const FVector2D& UV0)
	{
		int32 NewVertex = MeshBuilder.AddVertex(Position);
		MeshBuilder.SetNormalTangent(NewVertex, Normal, Tangent);
		MeshBuilder.SetUV(NewVertex, UV0);
	};

	FTriangleBuilderFunction TriangleBuilder = [&](int32 Index1, int32 Index2, int32 Index3)
	{
		MeshBuilder.AddIndex(Index1);
		MeshBuilder.AddIndex(Index2);
		MeshBuilder.AddIndex(Index3);
	};

	TArray<FVector> Verts = TArray<FVector>();

	{	// Add front cap
		Verts.Add(FVector(capDepth, 1.f, 0.f));
		for (int i = 1; i <= capSegments; i++)
		{
			float cos, sin;
			FMath::SinCos(&sin, &cos, GlobalVectorConstants::Pi.m128_f32[0] * i/((capSegments+1)*2));
			FVector CapVert = FVector(cos * capDepth, 0.f, sin * radius);
			for (int v = radVerts; v > 0; v--)
			{
				Verts.Add(
					CapVert.RotateAngleAxis(
					((float)v / (float)(radVerts))*360.f + rotationOffset,
						FVector(-1.f, 0.f, 0.f)));
			}
		}
	}

	{	// Transforming coordinates for front cap
		FVector StartPos = CenterPositions[0];
		FVector XCoord = FVector(StartPos - CenterPositions[1]);
		XCoord.Normalize();
		FVector YCoord = FVector(StartPos.X, StartPos.Y, 0.f);
		YCoord.Normalize();
		FVector ZCoord = FVector::CrossProduct(XCoord, YCoord);

		for (int i = 0; i < Verts.Num(); i++)
		{
			FVector vert = Verts[i];
			Verts[i] = FVector(XCoord * vert.X + YCoord * vert.Z + ZCoord * vert.Y) + StartPos;
		}
	}

	for (int i = 0; i < CenterPositions.Num(); i++)
	{
		FVector Center = CenterPositions[i];
		FVector Normal = FVector(0.f, 0.f, 0.f);
		if (i > 0)									Normal += Center - (CenterPositions)[i - 1];
		if (i < CenterPositions.Num() - 1)			Normal += (CenterPositions)[i + 1] - Center;

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

		// Transform into local coordinates.

		for (int v = 0; v < radVerts; v++)
		{
			float Angle = ((float)v / (float)(radVerts))*360.f + rotationOffset;
			FVector VertToAdd = Vert.RotateAngleAxis(Angle, Normal);
			Verts.Add(VertToAdd + Center);
		}
	}

	{	// Add back cap
		for (int i = 1; i <= capSegments; i++)
		{
			float cos, sin;
			FMath::SinCos(&sin, &cos, GlobalVectorConstants::Pi.m128_f32[0] * i/((capSegments+1)*2));
			FVector CapVert = FVector(sin * capDepth, 0.f, cos * radius);
			for (int v = radVerts; v > 0; v--)
			{
				Verts.Add(
					CapVert.RotateAngleAxis(
					((float)v / (float)(radVerts))*360.f + rotationOffset,
						FVector(1.f, 0.f, 0.f)));
			}
		}
		Verts.Add(FVector(capDepth, 1.f, 0.f));
	}

	{	// Transforming coordinates for back cap
		FVector EndPos = CenterPositions[CenterPositions.Num()-1];
		FVector XCoord = FVector(EndPos - CenterPositions[CenterPositions.Num()-2]);
		XCoord.Normalize();
		FVector YCoord = FVector(EndPos.X, EndPos.Y, 0.f);
		YCoord.Normalize();
		FVector ZCoord = FVector::CrossProduct(XCoord, YCoord);
		int offset = 1 + (CenterPositions.Num() + capSegments) * radVerts;
		int finalVert = offset + radVerts + 1;
		for (int i = offset; i < Verts.Num(); i++)
		{
			FVector vert = Verts[i];
			Verts[i] = FVector(XCoord * vert.X + YCoord * vert.Z + ZCoord * vert.Y) + EndPos;
		}
	}

	{	// Creating the center tris
		FVector Normal = FVector(CenterPositions[0] - CenterPositions[1]);
		Normal.Normalize();
		FRuntimeMeshTangent Tangent;
		Tangent.TangentX = (Verts[1] - Verts[0]);
		Tangent.TangentX.Normalize();
		for (int i = 0; i <= radVerts; i++)
		{
			VerticesBuilder(Verts[i], Normal, Tangent, FVector2D(0.0f, 0.0f));
			if (i > 1)
			{
				TriangleBuilder(0, i - 1, i);
			}
		}
		TriangleBuilder(0, radVerts, 1); // Close the gap
	}

	for (int i = 0; i < CenterPositions.Num() + (capSegments * 2) - 1; i++)
	{
		// Create the verts of the radius just ahead
		for (int v = 0; v < radVerts - 1; v++)
		{
			// Add the starting anchor vert
			int i1 = 1 + v + i * radVerts;
			int i2 = 1 + v + (i + 1) * radVerts;
			int i3 = 1 + v + 1 + (i + 1) * radVerts;
			int i4 = 1 + v + 1 + i * radVerts;

			FVector vert1 = Verts[i1];
			FVector vert2 = Verts[i2];
			FVector vert3 = Verts[i3];
			FVector vert4 = Verts[i4];

			if (v == 0)
			{
				FRuntimeMeshTangent Tangent;
				Tangent.TangentX = vert1 - vert2;
				Tangent.TangentX.Normalize();
				FVector Normal = FVector(vert2 - CenterPositions[FMath::Clamp(i - capSegments + 1, 0, CenterPositions.Num()-1)]);
				Normal.Normalize();
				VerticesBuilder(vert2, Normal, Tangent, FVector2D((float)(v) / (float)(radVerts), (float)(i) / (float)(CenterPositions.Num() - 1)));
			}

			FVector Normal = FVector(vert3 - CenterPositions[FMath::Clamp(i - capSegments + 1, 0, CenterPositions.Num()-1)]);
			Normal.Normalize();
			FRuntimeMeshTangent Tangent;
			Tangent.TangentX = vert4 - vert3;
			Tangent.TangentX.Normalize();

			VerticesBuilder(vert3, Normal, Tangent, FVector2D((float)(v) / (float)(radVerts), (float)(i) / (float)(CenterPositions.Num() - 1)));

			TriangleBuilder(i1, i2, i4);
			TriangleBuilder(i2, i3, i4);
		}
		int i1 = radVerts + i * radVerts;
		int i2 = radVerts + (i + 1) * radVerts;
		int i3 = 1 + (i + 1) * radVerts;
		int i4 = 1 + i * radVerts;

		TriangleBuilder(i1, i2, i4);
		TriangleBuilder(i2, i3, i4);
	}

	{	// Creating the center tris
		FVector Normal = FVector(CenterPositions[CenterPositions.Num()-1] - CenterPositions[CenterPositions.Num()-2]);
		Normal.Normalize();
		FRuntimeMeshTangent Tangent;
		Tangent.TangentX = Verts[Verts.Num()-2] - Verts[Verts.Num()-3];
		Tangent.TangentX.Normalize();
		int offset = (CenterPositions.Num() + capSegments*2 - 1) * radVerts + 1;
		VerticesBuilder(Verts[Verts.Num()-1], Normal, Tangent, FVector2D(1.f, 1.f));
		int finalVert = Verts.Num()-1;
		for (int i = offset; i < finalVert - 1; i++)
		{
			TriangleBuilder(finalVert, i + 1, i);
		}
		TriangleBuilder(finalVert, offset, finalVert-1); // Close the gap
	}

	Section->Commit();
}

void AAcroMeshActor::Hit(FVector Position, FVector Direction)
{
	float Damage = Direction.Size();
	UE_LOG(LogTemp, Warning, TEXT("Damage: %f"), Damage);
	float TotalDamage = Parent->ApplyDamage(Damage);
	if (TotalDamage > MaxDamage)
	{
		DestroyVisuals(Position, Direction);
		AAcroGameMode* GameMode = Cast<AAcroGameMode>(GetWorld()->GetAuthGameMode());
		GameMode->DeleteMesh(Parent);
		Destroy();
	}
	else
	{
		DamageVisuals(TotalDamage);
	}
}

void AAcroMeshActor::DestroyVisuals_Implementation(FVector Position, FVector Direction)
{
	UWorld* World = GetWorld();
	UGameplayStatics::PlaySoundAtLocation((UObject*)this, DestroySound, CenterPositions[CenterPositions.Num() / 2]);
	for (int i = 0; i < CenterPositions.Num(); i++)
	{
		FVector CenterPosition = CenterPositions[i];
		UParticleSystemComponent* Particles = UGameplayStatics::SpawnEmitterAtLocation(World, ParticleInstance, CenterPosition);
		float force = 5000.f;
		float distance = (Position - CenterPosition).Size();
		Particles->SetVectorParameter(FName(TEXT("ImpulseForce")), Direction*(force / distance));
	}
}

void AAcroMeshActor::DamageVisuals_Implementation(float DamageAmount)
{
	MeshMaterialInstance->SetScalarParameterValue(FName(TEXT("Cracked")), (float)DamageAmount / MaxDamage);
	UWorld* World = GetWorld();
	UGameplayStatics::PlaySoundAtLocation((UObject*)this, DamageSound, CenterPositions[CenterPositions.Num()/2]);
	for (int i = 0; i < CenterPositions.Num(); i += 2)
	{
		UGameplayStatics::SpawnEmitterAtLocation(World, DamageParticleInstance, CenterPositions[i]);
	}
}

void AAcroMeshActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAcroMeshActor, CenterPositions);
	DOREPLIFETIME(AAcroMeshActor, RuntimeMeshComponent);
}
