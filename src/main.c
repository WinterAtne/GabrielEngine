#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stb_image.h>

#include <stdio.h>

#include "sprite.h"
#include "texture.h"

const int WINDOW_SIZE_X = 1920;
const int WINDOW_SIZE_Y = 1080;
const float WINDOW_ASPECT = (float)WINDOW_SIZE_X/(float)WINDOW_SIZE_Y;
const float WINDOW_SCALE = 1;
const char* WINDOW_NAME = "Hello World!";

int main() {
	// Setup
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_SIZE_X, WINDOW_SIZE_Y, WINDOW_NAME, NULL, NULL);
	if (window == NULL) {
		printf("Failled to create Window");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y);
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

	sprites_initialize(WINDOW_SIZE_X, WINDOW_SIZE_Y, WINDOW_SCALE);

	Sprite* s0;
	sprite_make(&s0);
	sprite_transform_translate(s0, (vec3){1.0f, 0.5f, 0.0f});
	Texture tex = make_texture("resources/textures/test_text.png");
	sprite_texture_set(s0, tex);

	Sprite* s1;
	sprite_make(&s1);
	sprite_transform_translate(s1, (vec3){-1.0f, 0.5f, 0.0f});


	// Main Loop
	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);
		
		sprites_draw();

		glfwSwapBuffers(window);
	}

	// Tear Down
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
