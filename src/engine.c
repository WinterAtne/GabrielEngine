#include "engine.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "rendering.h"
#include "utils.h"

GLFWwindow* window;

void glfw_error_callback(int error, const char* description) {
	error(description);
}

void GLAPIENTRY glad_error_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	error(message);
}

int engine_init(
		int window_x,
		int window_y,
		float window_scale,
		const char* window_name,
		const float clear_color[4]) {


	if (!glfwInit()) {
		error("glfw failled initialization")
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwSwapInterval(1); // Vsync = true

	window = glfwCreateWindow(window_x, window_y, window_name, NULL, NULL);
	if (window == NULL) {
		error("Failled to initialize glfw")
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetErrorCallback(glfw_error_callback);
	gladLoadGL();
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(glad_error_callback, 0);
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
