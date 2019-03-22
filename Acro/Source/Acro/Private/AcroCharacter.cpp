// Copyright � 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "AcroCharacter.h"
#include "AcroGameMode.h"
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
								   bThrow(false)
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

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	ProjectilePool = NewObject<UProjectilePool>();
	ProjectilePool->SetupBP(TEXT("/Game/Blueprints/Snowball"), TEXT("/Game/FX/SnowballExplosion_P.SnowballExplosion_P"));
}

void AAcroCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority() && ProjectilePool != nullptr)
	{
		ProjectilePool->Initialize(GetWorld(), 4, 4);
	}
	else if (ProjectilePool == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ProjectilePool == nullptr. WTF Happened?"));
	}
}

void AAcroCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bIsDrawing)
	{
		// if (AcroMesh != NULL && AcroMesh->HasMeshActor())
		// {
		// 	DrawingMesh();
		// }
		if (HasAuthority())
		{
			DrawingMesh();
		}
		else
		{
			UpdateDrawPosition();
		}
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

	// Replicate to everyone
	DOREPLIFETIME(AAcroCharacter, ProjectilePool);
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
}

void AAcroCharacter::Move2DHorizontal(float Value)
{
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
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Server Throw Windup"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Client Throw Windup"));
	}
}

void AAcroCharacter::Throw()
{
	float MouseX;
	float MouseY;
	AAcroPlayerController *PlayerController = Cast<AAcroPlayerController>(GetController());
	if (PlayerController != nullptr)
	{
		FVector2D ActorScreenLocation;
		FVector ActorLocation = GetActorLocation();
		ActorLocation += FVector(0.f, 0.f, 32.f);
		FVector2D ActorGameLocation = GameCoordinateUtils::WorldToGameCoordinates(ActorLocation);

		PlayerController->ProjectWorldLocationToScreen(ActorLocation, ActorScreenLocation);
		PlayerController->GetMousePosition(MouseX, MouseY);
		FVector2D Direction = ActorScreenLocation - FVector2D(MouseX, MouseY);
		Direction.Normalize();

		if (HasAuthority())
		{
			ServerThrow(ActorGameLocation, Direction);
		}
		else
		{
			ClientThrow(ActorGameLocation, Direction);
		}
	}
}

void AAcroCharacter::ClientThrow_Implementation(FVector2D Position, FVector2D Direction)
{
	ServerThrow(Position, Direction);
}

void AAcroCharacter::ServerThrow(FVector2D Position, FVector2D Direction)
{
	TArray<UStaticMeshComponent *> Components;
	if (ProjectilePool == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ProjectilePool == nullptr. WTF Happened?"));
		return;
	}
	AProjectile *Projectile = ProjectilePool->Acquire(GetWorld());
	Projectile->Spawn(Position, Direction);
}

void AAcroCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	Jump();
}

void AAcroCharacter::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	StopJumping();
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
	UE_LOG(LogTemp, Warning, TEXT("BeginGeneratingMesh"));
	bIsDrawing = true;
	UpdateDrawPosition();
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
	AAcroGameMode* GameMode = Cast<AAcroGameMode>(GetWorld()->GetAuthGameMode());
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
	UE_LOG(LogTemp, Warning, TEXT("TODO: Remove Health From Character. Apply Direction Force. Camera Shake."));
}
