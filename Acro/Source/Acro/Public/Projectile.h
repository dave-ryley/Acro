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

	void Spawn(FVector2D GamePosition, FVector2D DirectionVector);
	void Setup(UProjectilePool* Pool);

private:
	bool active = false;
	UProjectilePool* ProjectilePool;
	FVector2D Direction;
	FVector2D Position;

	void Tick(float DeltaSeconds);
};

