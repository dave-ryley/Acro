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

	void SetupBP(const TCHAR * projectilePath);
	void Initialize(UWorld* World, int InitialSize, int IncrementalSize);

	AProjectile* Acquire(UWorld* World);
	void Release(AProjectile* Actor);

	virtual bool IsSupportedForNetworking() const override { return true; }

protected:
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

private:
	TSubclassOf<AProjectile> ActorClass;

	//UPROPERTY(Replicated)
	TArray<AProjectile*> Actors;

	int IncSize = 1;
	int CurIndex = 0;
};
