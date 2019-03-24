#include "Projectile.h"
#include "ProjectilePool.h"
#include "AcroMeshActor.h"
#include "AcroCharacter.h"
#include "GameCoordinateUtils.h"
#include "AcroDefinitions.h"
#include "Components/SphereComponent.h"

// Trace Function Taken from and modified: https://wiki.unrealengine.com/Trace_Functions
static FORCEINLINE bool Trace(
	UWorld* World,
	AActor* ActorToIgnore,
	const FVector& Start,
	const FVector& End,
	FHitResult& HitOut,
	ECollisionChannel CollisionChannel = ECC_Pawn,
	bool ReturnPhysMat = false
) {
	if (!World)
	{
		return false;
	}

	FCollisionQueryParams TraceParams(FName(TEXT("VictoreCore Trace")), true, ActorToIgnore);
	TraceParams.bTraceComplex = true;
	//TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = ReturnPhysMat;

	//Ignore Actors
	TraceParams.AddIgnoredActor(ActorToIgnore);

	//Re-initialize hit info
	HitOut = FHitResult(ForceInit);

	//Trace!
	World->LineTraceSingleByChannel(
		HitOut,		//result
		Start,	//start
		End, //end
		CollisionChannel, //collision channel
		TraceParams
	);

	//Hit any Actor?
	return (HitOut.GetActor() != NULL);
}

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
	bReplicates = false;

	USphereComponent* Sphere = CreateDefaultSubobject<USphereComponent>(FName("SphereComponent"));
	Sphere->SetWorldScale3D(FVector(1.f, 1.0f, 1.0f));
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	RootComponent = Sphere;
}

AProjectile::~AProjectile()
{
}

void AProjectile::Spawn(FVector2D GamePosition, FVector2D DirectionVector)
{
	active = true;
	Direction = DirectionVector;
	Position = GamePosition + (DirectionVector * 100.f);
	FVector WorldPosition = GameCoordinateUtils::GameToWorldCoordinates(Position);
	SetActorLocation(WorldPosition, false, nullptr, ETeleportType::TeleportPhysics);
}

void AProjectile::Setup(UProjectilePool* Pool)
{
	ProjectilePool = Pool;
}

void AProjectile::Tick(float DeltaSeconds)
{
	if (active)
	{
		if (HasAuthority())
		{
			Direction += FVector2D(0.f, GRAVITY * -DeltaSeconds);
			FVector OldWorldPosition = GameCoordinateUtils::GameToWorldCoordinates(Position);
			Position += (Direction * 1500.f * DeltaSeconds); // TODO: Replace 1500 with windup force
			FVector WorldPosition = GameCoordinateUtils::GameToWorldCoordinates(Position);
			FHitResult HitResult = FHitResult();
			if (Trace(GetWorld(), this, OldWorldPosition, WorldPosition, HitResult, ECC_WorldDynamic))
			{
				ProjectilePool->Explode(this);
				AActor* Actor = HitResult.GetActor();
				AAcroMeshActor* MeshActor = Cast<AAcroMeshActor>(Actor);
				if (MeshActor != nullptr)
				{
					MeshActor->Hit(WorldPosition, WorldPosition - OldWorldPosition);
					return;
				}
				AAcroCharacter* Character = Cast<AAcroCharacter>(Actor);
				if(Character != nullptr)
				{
					Character->Hit(WorldPosition - OldWorldPosition);
					return;
				}
			}
			else
			{
				SetActorLocation(WorldPosition, false, nullptr, ETeleportType::None);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Snowball moving on Client"));
		}
	}
}