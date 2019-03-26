// Copyright � 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "AcroCharacter.h"

#include "AcroGameMode.generated.h"

UCLASS(minimalapi)
class AAcroGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AAcroGameMode();
	virtual void InitGame(const FString & MapName, const FString & Options, FString & ErrorMessage) override;
	virtual void StartPlay() override;

	virtual void SaveMesh(UAcroMesh* Mesh);
	virtual void DeleteMesh(UAcroMesh* Mesh);

	void ThrowProjectile(FVector2D Position, FVector2D Direction);
	void PauseGame(bool value);

	TSubclassOf<AAcroCharacter> PlayerCharacterClass;
	UProjectilePool* ProjectilePool;

protected:
	TSubclassOf<AProjectile> ActorClass;
	TArray<UAcroMesh*> Meshes;
};
