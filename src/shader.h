#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct {
	GLuint program;
} Shader;

void shader_make(const char* vertexShader, const char* fragmentShader, Shader* shader);
void shader_activate(Shader* program);
void shader_delete(Shader* program);
