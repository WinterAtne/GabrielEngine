#include "input.h"

#include <GLFW/glfw3.h>

#include "glfwwindow.h"

bool IsKeyPressed(int keycode) {
	return glfwGetKey(window, keycode);
}
