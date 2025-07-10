#include "input.h"

#include <GLFW/glfw3.h>

#include "glfwwindow.h"

bool IsKeyPressed(int keycode) {
	return glfwGetKey(window, keycode);
}

// void MousePosition(vec2 position) {
// 	double x = 0.0f, y = 0.0f;
// 	glfwGetCursorPos(window, &x, &y);
//
// 	position[0] = (float)x;
// 	position[1] = (float)y;
// }
