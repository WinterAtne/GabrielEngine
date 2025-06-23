#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct {
	GLuint handle;
} Texture;

Texture texture_make(const char* texture_location);
void texture_bind(Texture texture, GLuint location);
