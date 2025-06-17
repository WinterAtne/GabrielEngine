#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stb_image.h>

#include <stdio.h>

#include "shader.h"
#include "sprite.h"

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

	// Texture
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	unsigned char* data = stbi_load("resources/textures/test_text.png", &width, &height, &nrChannels, 0);
	if (!data) {
		printf("no textures :(\n");
		exit(-1);
	} 

	// GLuint texture;
	// glGenTextures(1, &texture);
	// glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, texture);
	// // set the texture wrapping parameters
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// // set texture filtering parameters
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	// glGenerateMipmap(GL_TEXTURE_2D);
	// stbi_image_free(data);
	// glBindTexture(GL_TEXTURE_2D, 0);
	//
	// GLuint tex0Uni = glGetUniformLocation(shaderProgram->program, "tex0");
	// glUniform1i(tex0Uni, 0);

	initialize_sprites(WINDOW_SIZE_X, WINDOW_SIZE_Y, WINDOW_SCALE);
	Sprite* s0;
	Sprite* s1;
	make_sprite(&s0);
	sprite_transform_translate(s0, (vec3){1.0f, 0.5f, 0.0f});
	make_sprite(&s1);
	sprite_transform_translate(s1, (vec3){-1.0f, 0.5f, 0.0f});

	// Main Loop
	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);
		
		draw_sprites();

		glfwSwapBuffers(window);
	}

	// Tear Down
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
