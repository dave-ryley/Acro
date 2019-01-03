// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "MeshManager.h"
#include "Engine/GameEngine.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"

#define printf(text, ...) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, FString::Printf(TEXT(text), ##__VA_ARGS__))
#define RADIUS 1000.f // Need to move this to somewhere global

// Sets default values
AMeshManager::AMeshManager()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMeshManager::BeginPlay()
{
    Super::BeginPlay();
}

FVector GetCollisionVector(FVector worldLocation, FVector worldDirection)
{
    worldDirection *= 100000.f;
    worldDirection += worldLocation;
    FVector2D worldDirection2D = FVector2D(worldDirection.X, worldDirection.Y);

    float a = powf(worldDirection.X - worldLocation.X, 2) + powf(worldDirection.Y - worldLocation.Y, 2);
    float b = 2 * (worldDirection.X - worldLocation.X)*worldLocation.X + 2 * (worldDirection.Y - worldLocation.Y)*worldLocation.Y;
    float c = powf(worldLocation.X, 2) + powf(worldLocation.Y, 2) - powf(RADIUS, 2);
    float discriminant = b * b - 4 * a*c;
    if (discriminant > 0)
    {
        float scalar1 = (-b + sqrtf(discriminant)) / (2 * a);
        float scalar2 = (-b - sqrtf(discriminant)) / (2 * a);
        scalar1 = (scalar1 > 0 && scalar1 < 1) ? scalar1 : scalar2;
        scalar2 = (scalar2 > 0 && scalar2 < 1) ? scalar2 : scalar1;
        float scalar = (scalar1 < scalar2) ? scalar1 : scalar2;
        return ((worldDirection - worldLocation) * scalar) + worldLocation;
    }
    else
    {
        return FVector::ZeroVector;
    }
}

void AMeshManager::GenerateMesh(void)
{
    float mouseX;
    float mouseY;
    APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    playerController->GetMousePosition(mouseX, mouseY);
    FVector worldLocation = FVector::ZeroVector;
    FVector worldDirection = FVector::ZeroVector;
    playerController->DeprojectScreenPositionToWorld(mouseX, mouseY, worldLocation, worldDirection);
    FVector collisionVector = GetCollisionVector(worldLocation, worldDirection);
    if (collisionVector != FVector::ZeroVector)
    {
        _meshGenerating->ContinueGeneratingMesh(collisionVector);
    }
}

// Called every frame
void AMeshManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (_isGeneratingMesh)
    {
        GenerateMesh();
    }
}

void AMeshManager::BeginGeneratingMesh(FVector2D mousePosition)
{
    _meshGenerating = (AMeshPlatform*)GetWorld()->SpawnActor(AMeshPlatform::StaticClass());
    FVector worldLocation = FVector::ZeroVector;
    FVector worldDirection = FVector::ZeroVector;
    UGameplayStatics::GetPlayerController(GetWorld(), 0)->DeprojectScreenPositionToWorld(mousePosition.X, mousePosition.Y, worldLocation, worldDirection);
    FVector collisionVector = GetCollisionVector(worldLocation, worldDirection);
    if (collisionVector != FVector::ZeroVector)
    {
        _isGeneratingMesh = true;
        _meshGenerating->BeginGeneratingMesh(collisionVector);
    }
}

void AMeshManager::EndGeneratingMesh(FVector2D mousePosition)
{
    _isGeneratingMesh = false;
    FVector worldLocation = FVector::ZeroVector;
    FVector worldDirection = FVector::ZeroVector;
    UGameplayStatics::GetPlayerController(GetWorld(), 0)->DeprojectScreenPositionToWorld(mousePosition.X, mousePosition.Y, worldLocation, worldDirection);
    FVector collisionVector = GetCollisionVector(worldLocation, worldDirection);
    if (collisionVector != FVector::ZeroVector)
    {
        printf("Completing Mesh at Mouse Position %f, %f", mousePosition.X, mousePosition.Y);
        _meshGenerating->EndGeneratingMesh(collisionVector);
    }
}
