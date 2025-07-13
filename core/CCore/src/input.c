#include "input.h"

#include <GLFW/glfw3.h>

#include "glfwwindow.h"
#include "windowopt.h"

bool IsKeyPressed(int keycode) {
	return glfwGetKey(window, keycode);
}

MouseXY MousePosition() {
	double x = 0.0f, y = 0.0f;
	glfwGetCursorPos(window, &x, &y);

	x = ((double)DEFAULT_VIRTUAL_WINDOW_LENGTH/DEFAULT_WINDOW_LENGTH) + x*((double)DEFAULT_VIRTUAL_WINDOW_LENGTH/DEFAULT_WINDOW_LENGTH) - DEFAULT_VIRTUAL_WINDOW_LENGTH/2.0;
	y = ((double)DEFAULT_VIRTUAL_WINDOW_HEIGHT/DEFAULT_WINDOW_HEIGHT) + -(y*((double)DEFAULT_VIRTUAL_WINDOW_HEIGHT/DEFAULT_WINDOW_HEIGHT) - DEFAULT_VIRTUAL_WINDOW_HEIGHT/2.0);

	return (MouseXY){.x = x, .y = y};
}
