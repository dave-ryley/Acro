// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeshPlatform.h"
#include "MeshManager.generated.h"

UCLASS()
class ACRO_API AMeshManager : public AActor
{
    GENERATED_BODY()
    
public:	
    // Sets default values for this actor's properties
    AMeshManager();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

private:
    AMeshPlatform* _meshGenerating;
    bool _isGeneratingMesh = false;
    // Called on Tick if a mesh has begun gednerating
    void GenerateMesh(void);

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called from Blueprint on mouse input
    UFUNCTION(BlueprintCallable, Category = "RuntimeMesh")
    void BeginGeneratingMesh(FVector2D mousePosition);

    // Called from Blueprint on mouse release
    UFUNCTION(BlueprintCallable, Category = "RuntimeMesh")
    void EndGeneratingMesh(FVector2D mousePosition);

};
