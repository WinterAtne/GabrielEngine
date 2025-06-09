#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include <stdio.h>

#include "shader.h"

const int WINDOW_SIZE_X = 1920;
const int WINDOW_SIZE_Y = 1080;
const float WINDOW_ASPECT = (float)WINDOW_SIZE_X/(float)WINDOW_SIZE_Y;
const float WINDOW_SCALE = 64;
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

	// Setup Triangle
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f, // Bottom Left
		 0.5f, -0.5f, 0.0f, // Bottom Right
		-0.5f,  0.5f, 0.0f, // Top Left
		 0.5,   0.5f, 0.0f, // Top Right
	};

	GLuint indices[] = {
		0, 1, 2,
		2, 1, 3,
	};
	
	ShaderProgram* shaderProgram = MakeShaderProgram("resources/shaders/default.vert", "resources/shaders/default.frag");

	GLuint VAO, VBO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	ShaderProgramActivate(shaderProgram);

	mat4 model; glm_mat4_identity(model);

	mat4 view; glm_mat4_identity(view);
	vec3 viewTranslation;
	float viewTranslationValues[] = {1.5f, 1.0f, 0.0f};
	glm_vec3_make(viewTranslationValues, viewTranslation);
	glm_translate(view, viewTranslation);

	mat4 proj; glm_mat4_identity(proj);
	glm_ortho(-WINDOW_ASPECT * WINDOW_SCALE, WINDOW_ASPECT * WINDOW_SCALE, -WINDOW_SCALE, WINDOW_SCALE, 0.0f, 1.0f, proj);

	int modelLoc = glGetUniformLocation(shaderProgram->program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, *model);

	int viewLoc = glGetUniformLocation(shaderProgram->program, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, *view);

	int projLoc = glGetUniformLocation(shaderProgram->program, "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, *proj);
	
	// Main Loop
	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
	}

	// Tear Down
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	ShaderProgramDelete(shaderProgram);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
