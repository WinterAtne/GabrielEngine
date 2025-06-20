#include "engine.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "rendering.h"
#include "utils.h"

GLFWwindow* window;

int engine_init(
		int window_x,
		int window_y,
		float window_scale,
		const char* window_name,
		const float clear_color[4]) {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(window_x, window_y, window_name, NULL, NULL);
	if (window == NULL) {
		error("Failled to initialize glfw")
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, window_x, window_y);
	glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);

	sprites_initialize(window_x, window_y, window_scale);

	return 0;
}

int engine_should_close() {
	return glfwWindowShouldClose(window);
}

void engine_process() {
	glClear(GL_COLOR_BUFFER_BIT);

	sprites_draw();

	glfwPollEvents();
	glfwSwapBuffers(window);
}

void engine_end() {
	glfwDestroyWindow(window);
	glfwTerminate();
}
