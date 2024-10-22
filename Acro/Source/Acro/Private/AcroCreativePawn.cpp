// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "AcroCreativePawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "AcroPlayerController.h"
#include "../Public/AcroDefinitions.h"

AAcroCreativePawn::AAcroCreativePawn()
{
    bAllowTickBeforeBeginPlay = true;
    PrimaryActorTick.bCanEverTick = true;
    SetActorTickEnabled(false);

    // Create a camera boom attached to the root (capsule)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->bAbsoluteRotation = true; // Rotation of the character should not affect rotation of boom
    CameraBoom->bDoCollisionTest = false;
    CameraBoom->TargetArmLength = 1000.f;
    CameraBoom->SocketOffset = FVector(0.f, 0.f, 75.f);
    CameraBoom->RelativeRotation = FRotator(0.f, 180.f, 0.f);

    // Create a camera and attach to boom
    SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
    SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    SideViewCameraComponent->bUsePawnControlRotation = false; // We don't want the controller rotating the camera
}

void AAcroCreativePawn::Tick(float DeltaSeconds)
{
    if (bIsDrawing)
    {
        Super::Tick(DeltaSeconds);
        DrawingMesh();
    }
    else
    {
        SetActorTickEnabled(true);
    }
}

void AAcroCreativePawn::SetLevelSegment(FLevelSegment * Segment)
{
    LevelSegment = Segment;
}

void AAcroCreativePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    PlayerInputComponent->BindAction("Draw", IE_Pressed, this, &AAcroCreativePawn::DrawStarted);
    PlayerInputComponent->BindAction("Draw", IE_Released, this, &AAcroCreativePawn::DrawEnded);
    PlayerInputComponent->BindAxis("Move2DHorizontal", this, &AAcroCreativePawn::Move2DHorizontal);
    PlayerInputComponent->BindAxis("Move2DVertical", this, &AAcroCreativePawn::Move2DVertical);
}

FORCEINLINE FVector CollisionVector(FVector worldLocation, FVector worldDirection)
{
    worldDirection *= 100000.f;
    worldDirection += worldLocation;
    FVector2D worldDirection2D = FVector2D(worldDirection.X, worldDirection.Y);

    float a = powf(worldDirection.X - worldLocation.X, 2) + powf(worldDirection.Y - worldLocation.Y, 2);
    float b = 2 * (worldDirection.X - worldLocation.X)*worldLocation.X + 2 * (worldDirection.Y - worldLocation.Y)*worldLocation.Y;
    float c = powf(worldLocation.X, 2) + powf(worldLocation.Y, 2) - powf(LEVEL_RADIUS, 2);
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

void AAcroCreativePawn::DrawStarted()
{
    bIsDrawing = true;
    SetActorTickEnabled(true);
    float MouseX;
    float MouseY;
    AAcroPlayerController* PlayerController = Cast<AAcroPlayerController>(GetController());
    if (PlayerController != nullptr)
    {
        PlayerController->GetMousePosition(MouseX, MouseY);
        FVector WorldLocation = FVector::ZeroVector;
        FVector WorldDirection = FVector::ZeroVector;
        PlayerController->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection);
        FVector collisionVector = CollisionVector(WorldLocation, WorldDirection);
        if (collisionVector != FVector::ZeroVector)
        {
            LevelSegment->BeginGeneratingMesh(this, collisionVector);
        }
    }
}

void AAcroCreativePawn::DrawingMesh()
{
    float MouseX;
    float MouseY;
    AAcroPlayerController* PlayerController = Cast<AAcroPlayerController>(GetController());
    if (PlayerController != nullptr)
    {
        PlayerController->GetMousePosition(MouseX, MouseY);
        FVector WorldLocation = FVector::ZeroVector;
        FVector WorldDirection = FVector::ZeroVector;
        PlayerController->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection);
        FVector collisionVector = CollisionVector(WorldLocation, WorldDirection);
        if (collisionVector != FVector::ZeroVector)
        {
            LevelSegment->ContinueGeneratingMesh(collisionVector);
        }
    }
}

void AAcroCreativePawn::DrawEnded()
{
    bIsDrawing = false;
    SetActorTickEnabled(false);
    LevelSegment->CompleteGeneratingMesh();
}

void AAcroCreativePawn::Move2DHorizontal(float Value)
{
    if (Value != 0.0f)
    {
        FVector curLocation = GetActorLocation();
        Value /= 3.f;
        float rotationAmount = (-Value / LEVEL_CIRCUMFERENCE) * 360.f;
        FVector endLocation = curLocation.RotateAngleAxis(rotationAmount, FVector::UpVector);
        SetActorLocation(endLocation);
        float CameraRotation = FMath::RadiansToDegrees(atan2f(endLocation.Y, endLocation.X)) + 180.f;
        CameraBoom->SetRelativeRotationExact(FRotator(0.f, CameraRotation, 0.f));
    }
}

void AAcroCreativePawn::Move2DVertical(float Value)
{
    if (Value != 0.0f)
    {
        Value *= 3.f;
        FVector curLocation = GetActorLocation();
        curLocation += FVector(0.f, 0.f, Value);
        SetActorLocation(curLocation);
    }
}
