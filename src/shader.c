#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

#include "shader.h"

const char* ReadFile(const char* name) {
	FILE* source = fopen(name, "r");
	if (!source) {
		exit(-1);
	}

	fseek(source, 0L, SEEK_END);
	long size = ftell(source);
	rewind(source);

	if (!size) {
		exit(-1);
	}

	char* shader = calloc(size+1, sizeof(char));
	if (!shader) {
		exit(-1);
	}
	for (int i = 0; i < size +1; i++) {
		shader[i] = 0;
	}

	long err = fread((void*)shader, 1, size, source);
	if (err != size) {
		exit(-1);
	}

	fclose(source);
	return shader;
}

void shader_make(const char* vertexShaderName, const char* fragmentShaderName, Shader* shader) {
	GLint success;

	const char* vertexShaderSource = ReadFile(vertexShaderName);
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
		exit(-1);
	}
	free((void*)vertexShaderSource);
	vertexShaderSource = NULL;

	const char* fragmentShaderSource = ReadFile(fragmentShaderName);
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
		exit(-1);
	}
	free((void*)fragmentShaderSource);
	fragmentShaderSource = NULL;

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
		exit(-1);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void shader_activate(Shader* program) {
	glUseProgram(program->program);
}

void shader_delete(Shader* program) {
	glDeleteProgram(program->program);
}

