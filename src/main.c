#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stb_image.h>

#include <stdio.h>

#include "shader.h"
#include "render_queue.h"
#include "utils.h"

const int WINDOW_SIZE_X = 1920;
const int WINDOW_SIZE_Y = 1080;
const float WINDOW_ASPECT = (float)WINDOW_SIZE_X/(float)WINDOW_SIZE_Y;
const float WINDOW_SCALE = 1;
char* WINDOW_NAME = "Hello World!";

// GLuint VAO, texture;

void main_engine() {
	// glBindTexture(GL_TEXTURE_2D, texture);
	// glBindVertexArray(VAO);
	// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int main() {
	engine_setup(
			WINDOW_SIZE_X,
			WINDOW_SIZE_Y,
			WINDOW_SCALE,
			WINDOW_NAME,
			"resources/shaders/default.vert",
			"resources/shaders/default.frag");
	
	// Main Loop
	engine_process(main_engine);

	// Tear Down
	// glDeleteVertexArrays(1, &VAO);
	// glDeleteBuffers(1, &VBO);
	// glDeleteBuffers(1, &EBO);
	engine_teardown();
	// glfwTerminate();
	return 0;
}
