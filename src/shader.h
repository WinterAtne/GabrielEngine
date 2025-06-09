#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct {
	GLuint program;
} ShaderProgram;

ShaderProgram* MakeShaderProgram(const char* vertexShader, const char* fragmentShader);
void ShaderProgramActivate(ShaderProgram* program);
void ShaderProgramDelete(ShaderProgram* program);
