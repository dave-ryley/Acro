// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AcroMesh.h"
#include "ProjectilePool.h"
#include "AcroCharacter.generated.h"

UCLASS()
class ACRO_API AAcroCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

public:
	AAcroCharacter();

	void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	void Hit(FVector Direction);

protected:

	/** Called for side to side input */
	void Move2DHorizontal(float Val);

	void ThrowWindup();
	void Throw();

	/** Handle touch inputs. */
	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Handle touch stop event. */
	void TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	void DrawStarted();
	void DrawingMesh();
	void DrawEnded();
	void UpdateDrawPosition();

	UFUNCTION(Server, Unreliable, WithValidation)
	void SetDrawPosition(FVector NewPosition);
	void SetDrawPosition_Implementation(FVector NewPosition) { DrawPosition = NewPosition; }
	bool SetDrawPosition_Validate(FVector NewPosition) { return true; }

	UFUNCTION(Server, Reliable, WithValidation)
	void SetClientBeginDraw(FVector Position);
	void SetClientBeginDraw_Implementation(FVector Position);
	bool SetClientBeginDraw_Validate(FVector Position) { return true;  }

	UFUNCTION(Server, Reliable, WithValidation)
	void SetClientEndDraw();
	void SetClientEndDraw_Implementation();
	bool SetClientEndDraw_Validate() { return true; }

	void SetServerBeginDraw(FVector Position);
	void SetServerEndDraw();

	UFUNCTION(Server, Reliable, WithValidation)
	void ClientThrow(FVector2D Position, FVector2D Direction);
	void ClientThrow_Implementation(FVector2D Position, FVector2D Direction);
	bool ClientThrow_Validate(FVector2D Position, FVector2D Direction) { return true; }

	void ServerThrow(FVector2D Position, FVector2D Direction);

private:
	UAcroMesh* AcroMesh;

	bool bIsDrawing = false;
	bool bThrow = false;
	FVector DrawPosition;

	UPROPERTY(Replicated)
	UProjectilePool* ProjectilePool;

	TSubclassOf<AProjectile> ActorClass;
};
