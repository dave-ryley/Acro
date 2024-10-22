// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "GameCoordinateUtils.h"
#include "AcroDefinitions.h"

FORCEINLINE FVector CollisionVector(FVector WorldLocation, FVector WorldDirection)
{
	WorldDirection *= 100000.f;
	WorldDirection += WorldLocation;
	FVector2D WorldDirection2D = FVector2D(WorldDirection.X, WorldDirection.Y);

	float a = powf(WorldDirection.X - WorldLocation.X, 2) + powf(WorldDirection.Y - WorldLocation.Y, 2);
	float b = 2 * (WorldDirection.X - WorldLocation.X) * WorldLocation.X + 2 * (WorldDirection.Y - WorldLocation.Y) * WorldLocation.Y;
	float c = powf(WorldLocation.X, 2) + powf(WorldLocation.Y, 2) - powf(LEVEL_RADIUS, 2);
	float discriminant = b * b - 4 * a * c;
	if (discriminant > 0)
	{
		float scalar1 = (-b + sqrtf(discriminant)) / (2 * a);
		float scalar2 = (-b - sqrtf(discriminant)) / (2 * a);
		scalar1 = (scalar1 > 0 && scalar1 < 1) ? scalar1 : scalar2;
		scalar2 = (scalar2 > 0 && scalar2 < 1) ? scalar2 : scalar1;
		float scalar = (scalar1 < scalar2) ? scalar1 : scalar2;
		return ((WorldDirection - WorldLocation) * scalar) + WorldLocation;
	}
	else
	{
		return FVector::ZeroVector;
	}
}

FVector2D GameCoordinateUtils::ScreenToGameCoordinates(APlayerController* PlayerController, FVector2D ScreenCoordinates)
{
	FVector WorldCoordinates = ScreenToWorldCoordinates(PlayerController, ScreenCoordinates);
	return WorldToGameCoordinates(WorldCoordinates);
}

FVector2D GameCoordinateUtils::WorldToGameCoordinates(FVector WorldCoordinates)
{
	float radians = atan2(WorldCoordinates.Y, WorldCoordinates.X);
	FVector2D GameCoordinates = FVector2D(radians * LEVEL_RADIUS, WorldCoordinates.Z);
	//UE_LOG(LogTemp, Warning, TEXT("WorldToGameCoordinates, World: %f, %f - Game: %f"), WorldCoordinates.X, WorldCoordinates.Z, GameCoordinates.X);
	return GameCoordinates;
}

FVector2D GameCoordinateUtils::GameToScreenCoordinates(APlayerController* PlayerController, FVector2D GameCoordinates)
{
	FVector2D ScreenCoordinates;
	FVector WorldCoordinates = GameToWorldCoordinates(GameCoordinates);
	PlayerController->ProjectWorldLocationToScreen(WorldCoordinates, ScreenCoordinates);
	return ScreenCoordinates;
}

FVector GameCoordinateUtils::GameToWorldCoordinates(FVector2D GameCoordinates)
{
	float Angle = GameCoordinates.X / LEVEL_RADIUS;
	float XScale;
	float ZScale;
	FMath::SinCos(&ZScale, &XScale, Angle);
	FVector WorldCoordinates = FVector(LEVEL_RADIUS * XScale, LEVEL_RADIUS * ZScale, GameCoordinates.Y);
	//UE_LOG(LogTemp, Warning, TEXT("WorldToGameCoordinates, World: (%f, %f, %f) - Rotated By: %f Degrees"), WorldCoordinates.X, WorldCoordinates.Y, WorldCoordinates.Z, Angle);
	return WorldCoordinates;
}

FVector GameCoordinateUtils::ScreenToWorldCoordinates(APlayerController* PlayerController, FVector2D ScreenCoordinates)
{
	FVector WorldLocation = FVector::ZeroVector;
	FVector WorldDirection = FVector::ZeroVector;
	PlayerController->DeprojectScreenPositionToWorld(ScreenCoordinates.X, ScreenCoordinates.Y, WorldLocation, WorldDirection);
	FVector WorldCoordinates = CollisionVector(WorldLocation, WorldDirection);
	WorldToGameCoordinates(WorldCoordinates);
	return WorldCoordinates;
}
