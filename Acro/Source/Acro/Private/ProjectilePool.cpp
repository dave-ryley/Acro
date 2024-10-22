// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "ProjectilePool.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "UnrealNetwork.h"

UProjectilePool::UProjectilePool()
{

}

void UProjectilePool::SetupBP(const TCHAR * projectilePath)
{
	static ConstructorHelpers::FClassFinder<AProjectile> ActorBP(projectilePath);
	if (ActorBP.Succeeded())
	{
		ActorClass = ActorBP.Class;
	}
}

UProjectilePool::~UProjectilePool()
{
	for (int i = Actors.Num()-1; i >= 0; i--)
	{
		AProjectile* Actor = Actors[i];
		Actors.RemoveAt(i);
		if (Actor->IsValidLowLevel())
		{
			Actor->Destroy();
		}
	}
}

void UProjectilePool::Initialize(UWorld* World, int InitialSize, int IncrementalSize)
{
	IncSize = IncrementalSize;
	for (int i = 0; i < InitialSize; i++)
	{
		AProjectile* Actor = World->SpawnActor<AProjectile>(ActorClass);
		Actor->SetActorHiddenInGame(true);
		Actor->SetActorTickEnabled(false);
		Actors.Add(Actor);
	}
}

void UProjectilePool::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	//DOREPLIFETIME(UProjectilePool, Actors);
}

AProjectile* UProjectilePool::Acquire(UWorld* World)
{
	if (CurIndex >= Actors.Num())
	{
		for (int i = 0; i < IncSize; i++)
		{
			AProjectile* Actor = World->SpawnActor<AProjectile>(ActorClass);
			Actor->SetActorHiddenInGame(true);
			Actor->SetActorTickEnabled(false);
			Actors.Add(Actor);
		}
	}
	AProjectile* Actor = Actors[CurIndex];
	CurIndex++;
	Actor->SetActorHiddenInGame(false);
	Actor->SetActorTickEnabled(true);
	Actor->Setup(this);
	return Actor;
}

void UProjectilePool::Release(AProjectile* Actor)
{
	Actor->SetActorHiddenInGame(true);
	Actor->SetActorEnableCollision(false);
	Actor->SetActorTickEnabled(false);
	int Max = (CurIndex > Actors.Num()-1) ? Actors.Num()-1 : CurIndex;
	for (int i = 0; i <= Max; i++)
	{
		if (Actor == Actors[i])
		{
			CurIndex--;
			Actors[i] = Actors[CurIndex];
			Actors[CurIndex] = Actor;
			return;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Could not find return position for actor. Are you releasing to the wrong pool?"));
}

