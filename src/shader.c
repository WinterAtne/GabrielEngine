#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>

#include "shader.h"
#include "utils.h"


int shader_make(const char* vertexShaderSource, const char* fragmentShaderSource, Shader* shader) {
	GLint success;

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLint maxLength = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar* log = malloc(maxLength * sizeof(GLchar));

		glGetShaderInfoLog(vertexShader, maxLength, NULL, log);
		printf("Vertex Shader Failed to Compile!\n> OpenGL Error: %s", log);
		return -1;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLint maxLength = 0;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar* log = malloc(maxLength * sizeof(GLchar));

		glGetShaderInfoLog(fragmentShader, maxLength, NULL, log);
		printf("Fragment Shader Failed to Compile!\n> OpenGL Error: %s", log);
		return -1;
	}

	shader->program = glCreateProgram();
	glAttachShader(shader->program, vertexShader);
	glAttachShader(shader->program, fragmentShader);
	glLinkProgram(shader->program);
	glGetProgramiv(shader->program, GL_LINK_STATUS, &success);
	if (!success) {
		GLint maxLength = 0;
		glGetProgramiv(shader->program, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar* log = malloc(maxLength * sizeof(GLchar));;

		glGetProgramInfoLog(shader->program, maxLength, &maxLength, log);
		printf("Shader Failed to Link!\n> OpenGL Error: %s", log);
		free(log);
		return -1;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return 0;
}

void shader_activate(Shader* program) {
	glUseProgram(program->program);
}

void shader_delete(Shader* program) {
	glDeleteProgram(program->program);
}

