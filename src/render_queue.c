#include "render_queue.h"

#include <stdio.h>

#include "shader.h"
#include "utils.h"


GLFWwindow* window;
ShaderProgram* shaders;

void engine_setup(
		int window_size_x,
		int window_size_y,
		int window_scale,
		char* window_name,
		char* default_vertex_loc,
		char* default_fragment_loc) {
	// Setup
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(window_size_x, window_size_y, window_name, NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failled to create Window\n");
		glfwTerminate();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, window_size_x, window_size_y);
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

	// Shaders
	shaders = MakeShaderProgram(default_vertex_loc, default_fragment_loc);
	ShaderProgramActivate(shaders);
}

void engine_teardown() {
	ShaderProgramDelete(shaders);
}

void engine_process(void (*func)()) {
	if (!func) {
		ERROR("main function is null");
		exit(-1);
	}

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		func();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

ShaderProgram* get_shaders() {
	return shaders;
}
