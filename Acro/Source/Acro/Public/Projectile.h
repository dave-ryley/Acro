#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UProjectilePool;

UCLASS()
class AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();
	~AProjectile();

	void Setup(UProjectilePool* Pool);

	UFUNCTION(NetMulticast, Reliable)
	void Spawn(FVector2D GamePosition, FVector2D DirectionVector);
	void Spawn_Implementation(FVector2D GamePosition, FVector2D DirectionVector);

	UFUNCTION(NetMulticast, Reliable)
	void Explode();
	void Explode_Implementation();

	UPROPERTY(EditAnywhere)
	UParticleSystem* ParticleInstance;

private:
	bool active = false;
	UProjectilePool* ProjectilePool;
	FVector2D Direction;
	FVector2D Position;

	void Tick(float DeltaSeconds);
};
