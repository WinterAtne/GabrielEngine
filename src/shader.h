#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct {
	GLuint program;
} Shader;

int shader_make(const char* vertexShaderSource, const char* fragmentShaderSource, Shader* shader);
void shader_activate(Shader* program);
void shader_delete(Shader* program);
