#pragma once

#include "Engine/GameEngine.h"

#define LEVEL_RADIUS 2000.f
#define LEVEL_CIRCUMFERENCE LEVEL_RADIUS * 2 * PI

const FString SAVE_DIRECTORY_PATH = FString("SaveGames");

#define LEVEL_HEIGHT 500.f
#define LEVEL_WIDTH LEVEL_RADIUS / 8.f

#define VECTOR_LENGTH_THRESHOLD 50
#define VECTOR_CUTOFF_THRESHOLD 75

#define PI_2 PI*2
#define GRAVITY 0.f
