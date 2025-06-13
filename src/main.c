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

void main_engine() {
}

int main() {
	engine_setup(
			WINDOW_SIZE_X,
			WINDOW_SIZE_Y,
			WINDOW_SCALE,
			WINDOW_NAME,
			"resources/shaders/default.vert",
			"resources/shaders/default.frag");

	// Setup Triangle
	GLfloat vertices[] = {
		-0.5f,  0.5f,	0.0f, 0.0f, // Top Left
		 0.5,   0.5f,	1.0f, 0.0f, // Top Right
		-0.5f, -0.5f,	0.0f, 1.0f, // Bottom Left
		 0.5f, -0.5f,	1.0f, 1.0f, // Bottom Right
	};

	GLuint indices[] = {
		0, 1, 2,
		2, 1, 3,
	};
	
	GLuint VAO, VBO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// Model
	mat4 model; glm_mat4_identity(model);
	vec3 modelTranslation;
	float modelTranslationValues[] = {0.0f, 0.0f, 0.0f};
	glm_vec3_make(modelTranslationValues, modelTranslation);
	glm_translate(model, modelTranslation);

	// Camera
	mat4 view; glm_mat4_identity(view);
	vec3 viewTranslation;
	float viewTranslationValues[] = {0.0f, 0.0f, 0.0f};
	glm_vec3_make(viewTranslationValues, viewTranslation);
	glm_translate(view, viewTranslation);

	mat4 proj; glm_mat4_identity(proj);
	glm_ortho(
				-WINDOW_ASPECT * WINDOW_SCALE, WINDOW_ASPECT * WINDOW_SCALE, // X
				-WINDOW_SCALE, WINDOW_SCALE, // Y
				 0.0f, 1.0f, // Z
				 proj); // Dest

	int modelLoc = glGetUniformLocation(get_shaders()->program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, *model);

	int viewLoc = glGetUniformLocation(get_shaders()->program, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, *view);

	int projLoc = glGetUniformLocation(get_shaders()->program, "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, *proj);

	// Texture
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	unsigned char* data = stbi_load("resources/textures/test_text.png", &width, &height, &nrChannels, 0);
	if (!data) {
		printf("no textures :(\n");
		exit(-1);
	} 

	GLuint texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint tex0Uni = glGetUniformLocation(get_shaders()->program, "tex0");
	glUniform1i(tex0Uni, 0);

	// Main Loop
	engine_process(main_engine);

	// Tear Down
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	engine_teardown();
	glfwTerminate();
	return 0;
}
