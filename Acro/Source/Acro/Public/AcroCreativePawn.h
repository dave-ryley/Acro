// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "LevelSegment.h"
#include "AcroCreativePawn.generated.h"

UCLASS()
class ACRO_API AAcroCreativePawn : public APawn
{
    GENERATED_BODY()

    /** Side view camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* SideViewCameraComponent;

    /** Camera boom positioning the camera beside the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

protected:
    void DrawStarted();
    void DrawingMesh();
    void DrawEnded();

public:
    AAcroCreativePawn();

    virtual void Tick(float DeltaSeconds) override;

    void SetLevelSegment(FLevelSegment * Segment);

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void Move2DHorizontal(float Value);
    void Move2DVertical(float Value);

    FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }

    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

private:
    FLevelSegment* LevelSegment;

    bool bIsDrawing = false;
};
