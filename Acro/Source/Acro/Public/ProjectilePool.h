// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Projectile.h"
#include "CoreMinimal.h"
#include "ProjectilePool.generated.h"

UCLASS()
class ACRO_API UProjectilePool : public UObject
{
	GENERATED_BODY()
public:
	UProjectilePool();
	~UProjectilePool();

	void SetupBP(const TCHAR * projectilePath, const TCHAR * emitterPath);
	void Initialize(UWorld* World, int InitialSize, int IncrementalSize);

	AProjectile* Acquire(UWorld* World);
	void Release(AProjectile* Actor);
	void Explode(AProjectile* Actor);

	virtual bool IsSupportedForNetworking() const override { return true; }

protected:
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

private:
	TSubclassOf<AProjectile> ActorClass;
	UParticleSystem* ParticleInstance;

	//UPROPERTY(Replicated)
	TArray<AProjectile*> Actors;

	int IncSize = 1;
	int CurIndex = 0;
};
