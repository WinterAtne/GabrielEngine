#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stb_image.h>

#include "shader.h"

void engine_setup(
		int window_size_x,
		int window_size_y,
		int window_scale,
		char* window_name,
		char* default_vertex_loc,
		char* default_fragment_loc);

void engine_teardown();

void engine_process(void (*func)());

ShaderProgram* get_shaders();
