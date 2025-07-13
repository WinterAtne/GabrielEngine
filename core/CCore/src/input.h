#pragma once

#include <cglm/types.h>
#include <stdbool.h>

typedef struct {
	float x, y;
} MouseXY;

// Checks if the keycode is pressed.
bool IsKeyPressed(int keycode);
// Places the current mouse position into position.
MouseXY MousePosition();

