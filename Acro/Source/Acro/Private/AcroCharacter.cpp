// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "AcroCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/GameEngine.h"
#include "../Public/AcroDefinitions.h"

AAcroCharacter::AAcroCharacter()
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Don't rotate when the controller rotates.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Create a camera boom attached to the root (capsule)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->bAbsoluteRotation = true; // Rotation of the character should not affect rotation of boom
    CameraBoom->bDoCollisionTest = false;
    CameraBoom->TargetArmLength = 1000.f;
    CameraBoom->SocketOffset = FVector(0.f,0.f,75.f);
    CameraBoom->RelativeRotation = FRotator(0.f,180.f,0.f);

    // Create a camera and attach to boom
    SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
    SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    SideViewCameraComponent->bUsePawnControlRotation = false; // We don't want the controller rotating the camera

    // Configure character movement
    UCharacterMovementComponent* characterMovement = GetCharacterMovement();
    characterMovement->bOrientRotationToMovement = true; // Face in the direction we are moving..
    characterMovement->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // ...at this rotation rate
    characterMovement->GravityScale = 2.f;
    characterMovement->AirControl = 0.80f;
    characterMovement->JumpZVelocity = 1000.f;
    characterMovement->GroundFriction = 3.f;
    characterMovement->MaxWalkSpeed = 600.f;
    characterMovement->MaxFlySpeed = 600.f;

    // Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
    // are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

}

void AAcroCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    FVector curLocation = GetActorLocation();
    FVector distanceCheck = curLocation - FVector(0.f, 0.f, curLocation.Z);
    curLocation = (curLocation / distanceCheck.Size()) * LEVEL_RADIUS;
    SetActorLocation(curLocation);
    CameraBoom->RelativeRotation = FRotator(0.f, FMath::RadiansToDegrees(atan2f(curLocation.Y, curLocation.X)) + 180.f, 0.f);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AAcroCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    // set up gameplay key bindings
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
    PlayerInputComponent->BindAxis("Move2DHorizontal", this, &AAcroCharacter::Move2DHorizontal);

    PlayerInputComponent->BindTouch(IE_Pressed, this, &AAcroCharacter::TouchStarted);
    PlayerInputComponent->BindTouch(IE_Released, this, &AAcroCharacter::TouchStopped);
}

void AAcroCharacter::Move2DHorizontal(float Value)
{
    // add movement in that direction
    if (Value != 0.0f)
    {
        FVector curLocation = GetActorLocation();
        float rotationAmount = -Value / LEVEL_CIRCUMFERENCE * 360.f;
        FVector endLocation = curLocation.RotateAngleAxis(rotationAmount, FVector::UpVector);
        AddMovementInput(endLocation - curLocation, 1.f);
    }
}

void AAcroCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
    // jump on any touch
    Jump();
}

void AAcroCharacter::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
    StopJumping();
}

