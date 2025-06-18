#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct {
	GLuint handle;
} Texture;

Texture make_texture(const char* texture_location);
void bind_texture(Texture texture, GLuint location);
