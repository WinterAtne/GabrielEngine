#pragma once
#include "stdbool.h"

// Initializes OpenGL and a window.
extern int InitWindow();
// Closes the OpenGL context and window.
extern void DestroyWindow();
// Returns true when the OS has set the window to close.
extern bool ShouldDestroyWindow();
// Processes window events. Swaps buffers.
extern void ProcessWindow();

// COMING SOON
// Change window size and clear color.
