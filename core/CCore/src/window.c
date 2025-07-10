#include "window.h"

#include <assert.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "errors.h"
#include "window_options.h"

GLFWwindow* window = NULL;
bool isWindowInitialized = false;

/* ---- Private Functions ---- */
static void glfw_error_callback(int error, const char* description) {
	error(description);
}

static void GLAPIENTRY glad_error_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	error(message);
}

/* ---- Public Functions ---- */

extern int InitWindow() {
	if (!isWindowInitialized) {
		isWindowInitialized = true;
	} else {
		error("Window is already initialized")
		return -1;
	}

	int err = glfwInit();
	if (!err) {
		error("Failled to initialize glfw")
		return -1;
	}

	glfwSetErrorCallback(glfw_error_callback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OGL_VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OGL_VERSION_MINOR);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(DEFAULT_WINDOW_LENGTH, DEFAULT_WINDOW_HEIGHT, DEFAULT_WINDOW_NAME, NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		error("GLFW failled to create window");
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(BUFFER_SWAP_INTERVAL); // Vsync
	
	gladLoadGL();
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(glad_error_callback, 0);

	glViewport(0, 0, DEFAULT_WINDOW_LENGTH, DEFAULT_WINDOW_HEIGHT);
	glClearColor(
			DEFAULT_CLEAR_COLOR[0],
			DEFAULT_CLEAR_COLOR[1],
			DEFAULT_CLEAR_COLOR[2],
			DEFAULT_CLEAR_COLOR[3]);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	return 0;
}

extern void DestroyWindow() {
	assert(isWindowInitialized);
	glfwDestroyWindow(window);
	glfwTerminate();
}

extern bool ShouldDestroyWindow() {
	assert(isWindowInitialized);
	return glfwWindowShouldClose(window);
}

extern void ProcessWindow() {
	assert(isWindowInitialized);
	glfwPollEvents();
	glfwSwapBuffers(window);
}
