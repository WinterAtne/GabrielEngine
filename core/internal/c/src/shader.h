#pragma once

typedef struct {
	unsigned int program;
} Shader;

int shader_make(const char* vertexShaderSource, const char* fragmentShaderSource, Shader* shader);
void shader_activate(Shader* program);
void shader_delete(Shader* program);
