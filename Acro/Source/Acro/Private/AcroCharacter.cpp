// Copyright � 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "AcroCharacter.h"
#include "AcroGameMode.h"
#include "AcroVsGameMode.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/GameEngine.h"
#include "AcroDefinitions.h"
#include "AcroPlayerController.h"
#include "UnrealNetwork.h"
#include "GameCoordinateUtils.h"

AAcroCharacter::AAcroCharacter() : bIsDrawing(false),
	bThrow(false),
	bInMatch(false),
	bBlockInput(false)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

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

	// Configure character movement
	UCharacterMovementComponent *characterMovement = GetCharacterMovement();
	characterMovement->bOrientRotationToMovement = true;			// Face in the direction we are moving..
	characterMovement->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // ...at this rotation rate
	characterMovement->GravityScale = 2.f;
	characterMovement->AirControl = 0.80f;
	characterMovement->JumpZVelocity = 1000.f;
	characterMovement->GroundFriction = 3.f;
	characterMovement->MaxWalkSpeed = 600.f;
	characterMovement->MaxFlySpeed = 600.f;
}

void AAcroCharacter::BeginPlay()
{
	Super::BeginPlay();
	Health = 150.f;
	bBlockInput = false;
}

void AAcroCharacter::StartMatch_Implementation(FVector Position)
{
	SetActorLocation(Position);
	bInMatch = true;
	bBlockInput = true;
	Health = 150.f;
	OnMatchStarted.Broadcast();
	UE_LOG(LogTemp, Warning, TEXT("StartMatch."));
}

void AAcroCharacter::CountdownComplete()
{
	Health = 150.f;
	bBlockInput = false;
}

void AAcroCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bBlockInput) return;
	if (bIsDrawing)
	{
		if(IcePower > 0)
			if (HasAuthority())
			{
				DrawingMesh();
			}
			else
			{
				UpdateDrawPosition();
			}
		else
		{
			DrawEnded();
		}
	}
	else
	{
		IcePower = FMath::Clamp(IcePower + DeltaSeconds/2.f, 0.f, 1.f);
	}
	if (bThrow)
	{
		ThrowPower = FMath::Clamp(ThrowPower + DeltaSeconds*2.f, 0.f, 2.f);
	}
	FVector curLocation = GetActorLocation();
	FVector distanceCheck = curLocation - FVector(0.f, 0.f, curLocation.Z);
	curLocation = (curLocation / distanceCheck.Size()) * LEVEL_RADIUS;
	SetActorLocation(curLocation);
	CameraBoom->RelativeRotation = FRotator(0.f, FMath::RadiansToDegrees(atan2f(curLocation.Y, curLocation.X)) + 180.f, 0.f);
}

void AAcroCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAcroCharacter, Health);
}

void AAcroCharacter::SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent)
{
	// set up gameplay key bindings
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis("Move2DHorizontal", this, &AAcroCharacter::Move2DHorizontal);

	PlayerInputComponent->BindAction("Draw", IE_Pressed, this, &AAcroCharacter::DrawStarted);
	PlayerInputComponent->BindAction("Draw", IE_Released, this, &AAcroCharacter::DrawEnded);

	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &AAcroCharacter::ThrowWindup);
	PlayerInputComponent->BindAction("Throw", IE_Released, this, &AAcroCharacter::Throw);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &AAcroCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AAcroCharacter::TouchStopped);

	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AAcroCharacter::PauseGame);
}

void AAcroCharacter::Move2DHorizontal(float Value)
{
	if (bBlockInput) return;
	if (Value != 0.0f)
	{
		FVector curLocation = GetActorLocation();
		float rotationAmount = -Value / LEVEL_CIRCUMFERENCE * 360.f;
		FVector endLocation = curLocation.RotateAngleAxis(rotationAmount, FVector::UpVector);
		AddMovementInput(endLocation - curLocation, 1.f);
	}
}

void AAcroCharacter::ThrowWindup()
{
	if (bBlockInput) return;
	bThrow = true;
	ThrowPower = 0.5f;
}

void AAcroCharacter::Throw()
{
	float MouseX;
	float MouseY;
	AAcroPlayerController *PlayerController = Cast<AAcroPlayerController>(GetController());
	if (!bBlockInput && PlayerController != nullptr)
	{
		FVector2D ActorScreenLocation;
		FVector ActorLocation = GetActorLocation();
		ActorLocation += FVector(0.f, 0.f, 32.f);
		FVector2D ActorGameLocation = GameCoordinateUtils::WorldToGameCoordinates(ActorLocation);

		PlayerController->ProjectWorldLocationToScreen(ActorLocation, ActorScreenLocation);
		PlayerController->GetMousePosition(MouseX, MouseY);
		FVector2D Direction = ActorScreenLocation - FVector2D(MouseX, MouseY);
		Direction.Normalize();
		Direction *= ThrowPower;

		if (HasAuthority())
		{
			ServerThrow(ActorGameLocation, Direction);
		}
		else
		{
			ClientThrow(ActorGameLocation, Direction);
		}
	}
	bThrow = false;
	ThrowPower = 0.f;
}

void AAcroCharacter::ClientThrow_Implementation(FVector2D Position, FVector2D Direction)
{
	ServerThrow(Position, Direction);
}

void AAcroCharacter::ServerThrow(FVector2D Position, FVector2D Direction)
{
	AAcroGameMode* GameMode = Cast<AAcroGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->ThrowProjectile(Position, Direction);
}

void AAcroCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	Jump();
}

void AAcroCharacter::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	StopJumping();
}

void AAcroCharacter::Jump()
{
	if (bBlockInput) return;
	bPressedJump = true;
	JumpKeyHoldTime = 0.0f;
}

void AAcroCharacter::StopJumping()
{
	if (bBlockInput) return;
	bPressedJump = false;
	ResetJumpState();
}

void AAcroCharacter::PauseGame_Implementation()
{
	if (bBlockInput) return;
	AAcroGameMode* GameMode = Cast<AAcroGameMode>(GetWorld()->GetAuthGameMode());
	bBlockInput = true;
	GameMode->PauseGame(true);
}

void AAcroCharacter::UnpauseGame_Implementation()
{
	AAcroGameMode* GameMode = Cast<AAcroGameMode>(GetWorld()->GetAuthGameMode());
	bBlockInput = false;
	GameMode->PauseGame(false);
}

void AAcroCharacter::UpdateDrawPosition()
{
	float MouseX;
	float MouseY;
	AAcroPlayerController *PlayerController = Cast<AAcroPlayerController>(GetController());
	if (PlayerController != nullptr)
	{
		PlayerController->GetMousePosition(MouseX, MouseY);
		FVector v = GameCoordinateUtils::ScreenToWorldCoordinates(PlayerController, FVector2D(MouseX, MouseY));
		if (v != FVector::ZeroVector)
		{
			if (bInMatch)
			{
				IcePower -= (v - DrawPosition).Size()/750.f;
				IcePower = FMath::Clamp(IcePower, 0.f, 1.f);
			}
			DrawPosition = v;
			if (!HasAuthority())
			{
				SetDrawPosition(v);
			}
		}
	}
}

void AAcroCharacter::DrawStarted()
{
	if (bBlockInput) return;
	bIsDrawing = true;
	float temp = IcePower;
	UpdateDrawPosition();
	IcePower = temp;
	if (!HasAuthority())
	{
		SetClientBeginDraw(DrawPosition);
	}
	else
	{
		SetServerBeginDraw(DrawPosition);
	}
}

void AAcroCharacter::SetClientBeginDraw_Implementation(FVector Position)
{
	SetServerBeginDraw(Position);
}

void AAcroCharacter::SetServerBeginDraw(FVector Position)
{
	AcroMesh = NewObject<UAcroMesh>();
	AcroMesh->SpawnMeshActor(GetWorld());
	AcroMesh->BeginGeneratingMesh(Position);
	bIsDrawing = true;
}

void AAcroCharacter::DrawingMesh()
{
	UpdateDrawPosition();
	AcroMesh->ContinueGeneratingMesh(DrawPosition);
}

void AAcroCharacter::DrawEnded()
{
	if (bIsDrawing)
	{
		bIsDrawing = false;
		if (HasAuthority())
		{
			SetServerEndDraw();
		}
		else
		{
			SetClientEndDraw();
		}
	}
}

void AAcroCharacter::SetClientEndDraw_Implementation()
{
	bIsDrawing = false;
	SetServerEndDraw();
}

void AAcroCharacter::SetServerEndDraw()
{
    AAcroGameMode* GameMode = Cast<AAcroGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->SaveMesh(AcroMesh);
	AcroMesh = nullptr;
}

void AAcroCharacter::Hit(FVector Direction)
{
	float Damage = Direction.Size();
	UE_LOG(LogTemp, Warning, TEXT("TODO: Camera Shake."));
	GetCharacterMovement()->AddImpulse(Direction * 10000);
	if (bInMatch)
	{
		Health -= Damage;
		if (Health <= 0)
		{
			AAcroVsGameMode* GameMode = Cast<AAcroVsGameMode>(GetWorld()->GetAuthGameMode());
			GameMode->LoseGame(this);
		}
	}
}

void AAcroCharacter::WinGame_Implementation()
{
	bBlockInput = true;
	DrawEnded();
	OnGameWin.Broadcast();
}

void AAcroCharacter::LoseGame_Implementation()
{
	bBlockInput = true;
	DrawEnded();
	OnGameLose.Broadcast();
}