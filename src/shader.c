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

	const char* rtrn = malloc(size+1);
	if (!rtrn) {
		exit(-1);
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

	ShaderProgram* rtrn = malloc(sizeof(ShaderProgram));

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	rtrn->program = glCreateProgram();
	glAttachShader(rtrn->program, vertexShader);
	glAttachShader(rtrn->program, fragmentShader);
	glLinkProgram(rtrn->program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	

	free((void*)vertexShaderSource);
	free((void*)fragmentShaderSource);
	return rtrn;
}

void Activate(ShaderProgram *program) {
	glUseProgram(program->program);
}

void Delete(ShaderProgram *program) {
	glDeleteProgram(program->program);
}

