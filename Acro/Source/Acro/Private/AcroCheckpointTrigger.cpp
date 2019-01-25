// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "../Public/AcroCheckpointTrigger.h"
#include "../Public/AcroGameMode.h"
#include "../Public/AcroPlayerState.h"
#include "../Public/AcroPlayerController.h"
#include "../Public/AcroCharacter.h"
#include "Engine/GameEngine.h"
#include "Runtime/Engine/Classes/Components/SphereComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "../Public/AcroDefinitions.h"

AAcroCheckpointTrigger::AAcroCheckpointTrigger(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	SetActorHiddenInGame(false);
	USphereComponent* Visual = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, FName("Sphere"));
	Visual->bHiddenInGame = false;
	Visual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = Visual;

	UStaticMeshComponent* SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere Cisual"));
	SphereMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	GetCollisionComponent()->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMeshAsset.Succeeded()) {
		SphereMesh->SetStaticMesh(SphereMeshAsset.Object);
		SphereMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		SphereMesh->SetWorldScale3D(FVector(0.8f));
		SphereMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AAcroCheckpointTrigger::SetCheckPointType(bool Validator)
{
	if (Validator)
	{
		OnActorBeginOverlap.AddDynamic(this, &AAcroCheckpointTrigger::OnEnterLevelValidation);
	}
	else
	{
		OnActorBeginOverlap.AddDynamic(this, &AAcroCheckpointTrigger::OnEnterLevelComplete);
	}
}

void AAcroCheckpointTrigger::OnEnterLevelValidation(AActor * OverlappedActor, AActor * OtherActor)
{
	if (OtherActor && (OtherActor != this))
	{
		AAcroGameMode* GameMode = Cast<AAcroGameMode>(GetWorld()->GetAuthGameMode());
		GameMode->ValidateLevelSegment();
	}
}

void AAcroCheckpointTrigger::OnEnterLevelComplete(AActor * OverlappedActor, AActor * OtherActor)
{
	if (OtherActor && (OtherActor != this))
	{
		AAcroPlayerController* PlayerController = Cast<AAcroPlayerController>(OtherActor->GetInstigatorController());
		AAcroGameMode* GameMode = Cast<AAcroGameMode>(GetWorld()->GetAuthGameMode());
		GameMode->EnterCreativeMode();
	}
}
