#pragma once

#include "Engine/GameEngine.h"

#define printf(text, ...) UE_LOG(LogTemp, Warning, TEXT(text), __VA_ARGS__); if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, FString::Printf(TEXT(text), __VA_ARGS__))
#define print(text) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, text)

#define LEVEL_RADIUS 2000.f
#define LEVEL_CIRCUMFERENCE LEVEL_RADIUS * 2 * PI

#define LEVEL_HEIGHT 500.f
#define LEVEL_WIDTH LEVEL_RADIUS / 8.f

#define SAVE_DIRECTORY_PATH "SaveGames"

#define VECTOR_LENGTH_THRESHOLD 50
