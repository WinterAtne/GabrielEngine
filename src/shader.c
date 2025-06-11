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

	char* rtrn = calloc(size+1, sizeof(char));
	if (!rtrn) {
		exit(-1);
	}
	for (int i = 0; i < size +1; i++) {
		rtrn[i] = 0;
	}

	long err = fread((void*)rtrn, 1, size, source);
	if (err != size) {
		exit(-1);
	}

	fclose(source);
	return rtrn;
}

ShaderProgram* MakeShaderProgram(const char* vertexShaderName, const char* fragmentShaderName) {
	const char* vertexShaderSource = ReadFile(vertexShaderName);
	const char* fragmentShaderSource = ReadFile(fragmentShaderName);
	GLint success;

	ShaderProgram* rtrn = malloc(sizeof(ShaderProgram));

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

	rtrn->program = glCreateProgram();
	glAttachShader(rtrn->program, vertexShader);
	glAttachShader(rtrn->program, fragmentShader);
	glLinkProgram(rtrn->program);
	glGetProgramiv(rtrn->program, GL_LINK_STATUS, &success);
	if (!success) {
		GLint maxLength = 0;
		glGetProgramiv(rtrn->program, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar* log = malloc(maxLength * sizeof(GLchar));;

		glGetProgramInfoLog(rtrn->program, maxLength, &maxLength, log);
		printf("Shader Failed to Link!\n> OpenGL Error: %s", log);
		exit(-1);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	

	free((void*)vertexShaderSource);
	free((void*)fragmentShaderSource);
	return rtrn;
}

void ShaderProgramActivate(ShaderProgram *program) {
	glUseProgram(program->program);
}

void ShaderProgramDelete(ShaderProgram *program) {
	glDeleteProgram(program->program);
	free(program);
}

