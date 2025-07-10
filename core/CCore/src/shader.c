#include "shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

Shader NewShader(const char* vertex, const char* fragment) {
	Shader shader;
	GLint success;

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertex, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLint maxLength = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar* log = malloc(maxLength * sizeof(GLchar));

		glGetShaderInfoLog(vertexShader, maxLength, NULL, log);
		printf("Vertex Shader Failed to Compile!\n OpenGL Error: %s", log);
		free(log);
		return (Shader){};
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragment, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLint maxLength = 0;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar* log = malloc(maxLength * sizeof(GLchar));

		glGetShaderInfoLog(fragmentShader, maxLength, NULL, log);
		printf("Fragment Shader Failed to Compile!\n OpenGL Error: %s", log);
		free(log);
		return (Shader){};
	}

	shader.handle = glCreateProgram();
	glAttachShader(shader.handle, vertexShader);
	glAttachShader(shader.handle, fragmentShader);
	glLinkProgram(shader.handle);
	glGetProgramiv(shader.handle, GL_LINK_STATUS, &success);
	if (!success) {
		GLint maxLength = 0;
		glGetProgramiv(shader.handle, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar* log = malloc(maxLength * sizeof(GLchar));;

		glGetProgramInfoLog(shader.handle, maxLength, &maxLength, log);
		printf("Shader Failed to Link!\n OpenGL Error: %s", log);
		free(log);
		return (Shader){};
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shader;
}

void UseShader(Shader* shader) {
	glUseProgram(shader->handle);
}
