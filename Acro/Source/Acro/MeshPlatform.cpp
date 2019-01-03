// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "MeshPlatform.h"

#define CREATEBOX_NUMVERTS 24
#define CREATEBOX_NUMTRIS 36
#define VECTOR_LENGTH_THRESHOLD 50
#define printf(text, ...) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, FString::Printf(TEXT(text), ##__VA_ARGS__))

// Sets default values
AMeshPlatform::AMeshPlatform()
    : BoxSize(25.0f, 25.0f, 25.0f)
{
    bRunGenerateMeshesOnBeginPlay = false; // turning these off so it can be called from MeshManager
    bRunGenerateMeshesOnConstruction = false; // turning these off so it can be called from MeshManager

    // Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

}

void AMeshPlatform::BeginGeneratingMesh(FVector startingPos)
{
    _centerPositions.push_back(startingPos);
    _centerPositionsCount = 1;
}

void AMeshPlatform::ContinueGeneratingMesh(FVector pos)
{
    if (_centerPositionsCount < 2)
    {
        _centerPositions.push_back(pos);
        _centerPositionsCount += 1;
    }
    else
    {
        FVector lengthVector = _centerPositions[_centerPositionsCount - 2] - _centerPositions[_centerPositionsCount - 1];
        float magnitude = lengthVector.Size();
        if (magnitude > VECTOR_LENGTH_THRESHOLD)
        {
            _centerPositions.push_back(pos);
            _centerPositionsCount += 1;
        }
        else
        {
            _centerPositions[_centerPositionsCount - 1] = pos;
        }
    }
    GenerateMeshes();
}

void AMeshPlatform::EndGeneratingMesh(FVector endPos)
{
    //ContinueGeneratingMesh(endPos);
}

void AMeshPlatform::GenerateMeshes_Implementation()
{
    FRuntimeMeshDataPtr Data = GetRuntimeMeshComponent()->GetOrCreateRuntimeMesh()->GetRuntimeMeshData();
    Data->CreateMeshSection(0, false, false, 1, false, true, EUpdateFrequency::Average);

    auto Section = Data->BeginSectionUpdate(0);
    CreateCustomMesh(*Section.Get());
    Section->Commit();
}

void AMeshPlatform::CreateCustomMesh(FRuntimeMeshAccessor& MeshBuilder)
{
    float radius = 50.f;
    int radVerts = 4;
    float vertOffset = 0.f;

    /*MeshBuilder.EmptyVertices(2 * radVerts * (_centerPositionsCount-1) + 2*radVerts);
    MeshBuilder.EmptyIndices((2 * radVerts * (_centerPositionsCount-1) + 2*(radVerts - 2))*3);*/
    MeshBuilder.EmptyVertices(CREATEBOX_NUMVERTS);
    MeshBuilder.EmptyIndices(CREATEBOX_NUMTRIS);
    FVerticesBuilderFunction VerticesBuilder = [&](const FVector& Position, const FVector& Normal, const FRuntimeMeshTangent& Tangent, const FVector2D& UV0)
    {
        int32 NewVertex = MeshBuilder.AddVertex(Position);
        MeshBuilder.SetNormalTangent(NewVertex, Normal, Tangent);
        MeshBuilder.SetUV(NewVertex, UV0);
    };

    FTrianglesBuilderFunction TrianglesBuilder = [&](int32 Index)
    {
        MeshBuilder.AddIndex(Index);
    };

    //CreatePlatformMesh(radius, radVerts, vertOffset, VerticesBuilder, MeshBuilder);
    CreateBoxMesh(BoxSize, VerticesBuilder, TrianglesBuilder);
}

void AMeshPlatform::CreatePlatformMesh(float radius, int radVerts, float offset, FVerticesBuilderFunction VerticesBuilder, FRuntimeMeshAccessor& MeshBuilder)
{
    std::vector<std::vector<FVector>> vertList = std::vector<std::vector<FVector>>(_centerPositionsCount);
    int vertsAdded = 0;
    for (int i = 0; i < _centerPositionsCount; i++)
    {
        // Get the current center and determine the coordinates based on world center and neighbours
        FVector center = _centerPositions[i];
        FVector normal = FVector(0.f, 0.f, 0.f);
        if (i > 0)                                      normal += center - _centerPositions[i - 1];
        if (i < _centerPositionsCount - 1)              normal += _centerPositions[i + 1] - center;

        normal /= normal.Size(); // Dividing by magnitude to normalize
        FVector tangent = center - FVector(0.f, 0.f, center.Z); // Flattening the center vector into 2D to get the tangent
        tangent /= tangent.Size(); // Dividing by magnitude to normalize
        FVector cross = FVector::CrossProduct(tangent, normal);

        std::vector<FVector> verts = std::vector<FVector>(radVerts);
        vertList[i] = verts;

        FRuntimeMeshTangent Tangent;
        Tangent.TangentX = tangent;
        for (int v = 0; v < radVerts; v++)
        {
            FVector vert = tangent * radius;
            vert = vert.RotateAngleAxis((v / radVerts + 1) * 360 + offset, normal);
            vert += center;
            vertList[i][v] = vert;
            if (i > 0 && v > 0)
            {
                FVector vert1 = vertList[i-1][v-1];
                FVector vert2 = vertList[i][v-1]; 
                FVector vert3 = vertList[i][v];
                FVector vert4 = vertList[i-1][v];
                FVector vertNormal = FVector::CrossProduct(vert2 - vert1, vert4 - vert1);
                vertNormal /= vertNormal.Size();
                FVector vertTangent = vert2 - vert1;
                vertTangent /= vertTangent.Size();
                Tangent.TangentX = vertTangent;
                VerticesBuilder(vert1, vertNormal, Tangent, FVector2D((v-1) / radVerts, (i - 1) / _centerPositionsCount - 1));
                VerticesBuilder(vert2, vertNormal, Tangent, FVector2D((v - 1) / radVerts, i / _centerPositionsCount - 1));
                VerticesBuilder(vert3, vertNormal, Tangent, FVector2D(v / radVerts, i / _centerPositionsCount - 1));
                VerticesBuilder(vert4, vertNormal, Tangent, FVector2D(v / radVerts, (i - 1) / _centerPositionsCount - 1));
                MeshBuilder.AddTriangle(vertsAdded + 0, vertsAdded + 1, vertsAdded + 3);
                MeshBuilder.AddTriangle(vertsAdded + 1, vertsAdded + 2, vertsAdded + 3);
                vertsAdded += 4;
                if (v == radVerts-1)
                {
                    //VerticesBuilder(vert, normal, Tangent, FVector2D(v / radVerts, i / _centerPositionsCount - 1));
                    //VerticesBuilder(vert, normal, Tangent, FVector2D(v / radVerts, i / _centerPositionsCount - 1));
                    //VerticesBuilder(vert, normal, Tangent, FVector2D(v / radVerts, i / _centerPositionsCount - 1));
                    //VerticesBuilder(vert, normal, Tangent, FVector2D(v / radVerts, i / _centerPositionsCount - 1));
                    //MeshBuilder.AddTriangle(v + (i - 1) * radVerts, v + i * radVerts, (i - 1) * radVerts);
                    //MeshBuilder.AddTriangle(v + i * radVerts, i * radVerts, (i - 1) * radVerts);
                }
            }
            /*if ((i == 0 || i == _centerPositionsCount-1) && v > 1) // TODO: create these all at the start and end.
            {
                MeshBuilder.AddTriangle(0 + i * radVerts, v - 1 + i * radVerts, v + i * radVerts);
            }*/
        }
    }
}

void AMeshPlatform::ConvertQuadToTriangles(TFunction<void(int32 Index)> TrianglesBuilder, int32 Vert0, int32 Vert1, int32 Vert2, int32 Vert3)
{
    TrianglesBuilder(Vert0);
    TrianglesBuilder(Vert1);
    TrianglesBuilder(Vert3);

    TrianglesBuilder(Vert1);
    TrianglesBuilder(Vert2);
    TrianglesBuilder(Vert3);
}

void AMeshPlatform::CreateBoxMesh(FVector BoxRadius, FVerticesBuilderFunction VerticesBuilder, FTrianglesBuilderFunction TrianglesBuilder)
{
    int radVerts = 8;
    float radius = 35.f;
    float rotationOffset = 45.f;

    std::vector<FVector> verts = std::vector<FVector>(radVerts*_centerPositionsCount);
    for (int i = 0; i < _centerPositionsCount; i++)
    {
        FVector center = _centerPositions[i];
        FVector normal = FVector(0.f, 0.f, 0.f);
        if (i > 0)                                      normal += center - _centerPositions[i - 1];
        if (i < _centerPositionsCount - 1)              normal += _centerPositions[i + 1] - center;

        float normalSize = normal.Size();
        if (normalSize > 0)
        {
            normal /= normalSize; // Dividing by magnitude to normalize
        }
        else
        {
            normal = FVector(0.f, 0.f, 1.f);
        }
        FVector tangent = FVector(center.X, center.Y, 0.f); // Flattening the center vector into 2D to get the tangent
        float tangentSize = tangent.Size();
        if (tangentSize > 0)
        {
            tangent /= tangentSize; // Dividing by magnitude to normalize
        }
        FVector vert = tangent * radius;

        int iOffset = i * radVerts;
        for (int v = 0; v < radVerts; v++)
        {
            //float distanceFromEdge = abs(abs(v - radVerts / 2.f) - radVerts);
            //float cosValue = FMath::Clamp(distanceFromEdge, 0.f, 3.f)/3.f * PI/2;
            //float modifier = cosf(cosValue);
            //vert = tangent * radius * modifier;
            verts[v + iOffset] = vert.RotateAngleAxis(((float)v / (float)(radVerts + 1))*360.f + rotationOffset, normal) + center;
        }
    }

    FVector Normal;
    FRuntimeMeshTangent Tangent;

    Normal = FVector::CrossProduct(verts[2] - verts[0], verts[1] - verts[0]);
    Normal /= Normal.Size();
    Tangent.TangentX = (verts[1] - verts[0]);
    Tangent.TangentX.Normalize();
    for (int i = 0; i < radVerts; i++)
    {
        VerticesBuilder(verts[i], Normal, Tangent, FVector2D(0.0f, 0.0f));
        if (i > 1)
        {
            TrianglesBuilder(0);
            TrianglesBuilder(i-1);
            TrianglesBuilder(i);
        }
    }    
    int triangleVertOffset = radVerts;

    for (int i = 1; i < _centerPositionsCount; i++)
    {
        FVector center = _centerPositions[i];
        FVector normal = FVector(0.f, 0.f, 0.f);
        if (i > 0)                                      normal += center - _centerPositions[i - 1];
        if (i < _centerPositionsCount - 1)              normal += _centerPositions[i + 1] - center;

        normal /= normal.Size(); // Dividing by magnitude to normalize
        FVector tangent = FVector(center.X, center.Y, 0.f); // Flattening the center vector into 2D to get the tangent
        tangent /= tangent.Size(); // Dividing by magnitude to normalize
        FVector vert = tangent * 25.f;

        int indexOffset = i * radVerts;

        for (int v = 1; v < radVerts; v++)
        {
            FVector vert1 = verts[v - 1 + (i - 1)*radVerts];
            FVector vert2 = verts[v - 1 + i * radVerts];
            FVector vert3 = verts[v + i * radVerts];
            FVector vert4 = verts[v + (i - 1)*radVerts];

            Normal = FVector::CrossProduct(vert4 - vert1, vert2 - vert1);
            Normal /= Normal.Size();
            Tangent.TangentX = vert2 - vert1;
            Tangent.TangentX.Normalize();
            VerticesBuilder(vert1, Normal, Tangent, FVector2D((float)(v - 1) / (float)(radVerts), (i - 1) / (float)(_centerPositionsCount - 1)));
            VerticesBuilder(vert2, Normal, Tangent, FVector2D((float)(v - 1) / (float)(radVerts), (float)(i) / (float)(_centerPositionsCount - 1)));
            VerticesBuilder(vert3, Normal, Tangent, FVector2D((float)(v) / (float)(radVerts), (float)(i) / (float)(_centerPositionsCount - 1)));
            VerticesBuilder(vert4, Normal, Tangent, FVector2D((float)(v) / (float)(radVerts), (float)(i - 1) / (float)(_centerPositionsCount - 1)));
            ConvertQuadToTriangles(TrianglesBuilder, 0 + triangleVertOffset, 1 + triangleVertOffset, 2 + triangleVertOffset, 3 + triangleVertOffset);
            triangleVertOffset += 4;
        }
        FVector vert1 = verts[radVerts-1 + (i - 1)*radVerts];
        FVector vert2 = verts[radVerts-1 + i * radVerts];
        FVector vert3 = verts[i * radVerts];
        FVector vert4 = verts[(i - 1)*radVerts];

        Normal = FVector::CrossProduct(vert4 - vert1, vert2 - vert1);
        Normal /= Normal.Size();
        Tangent.TangentX = vert2 - vert1;
        Tangent.TangentX.Normalize();
        VerticesBuilder(vert1, Normal, Tangent, FVector2D((float)(radVerts - 1) / (float)(radVerts), (i - 1) / (float)(_centerPositionsCount - 1)));
        VerticesBuilder(vert2, Normal, Tangent, FVector2D((float)(radVerts - 1) / (float)(radVerts), (float)(i) / (float)(_centerPositionsCount - 1)));
        VerticesBuilder(vert3, Normal, Tangent, FVector2D((float)0.f, (float)(i) / (float)(_centerPositionsCount - 1)));
        VerticesBuilder(vert4, Normal, Tangent, FVector2D((float)0.f, (float)(i - 1) / (float)(_centerPositionsCount - 1)));
        ConvertQuadToTriangles(TrianglesBuilder, 0 + triangleVertOffset, 1 + triangleVertOffset, 2 + triangleVertOffset, 3 + triangleVertOffset);
        triangleVertOffset += 4;
    }

    int start = (radVerts * (_centerPositionsCount - 1));

    Normal = FVector::CrossProduct(verts[start + 1] - verts[start], verts[start + 2] - verts[start]);
    Normal /= Normal.Size();
    Tangent.TangentX = (verts[start + 2] - verts[start]);
    Tangent.TangentX.Normalize();
    for (int i = 0; i < radVerts; i++)
    {
        VerticesBuilder(verts[start + i], Normal, Tangent, FVector2D(0.0f, 0.0f));
        if (i > 1)
        {
            TrianglesBuilder(triangleVertOffset + 0);
            TrianglesBuilder(triangleVertOffset + i);
            TrianglesBuilder(triangleVertOffset + i - 1);
        }
    }
}
