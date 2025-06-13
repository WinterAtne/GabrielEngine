#pragma once

#include <cglm/cglm.h>

#include "shader.h"


// Engine
void engine_setup(
		int window_size_x,
		int window_size_y,
		int window_scale,
		char* window_name,
		char* default_vertex_loc,
		char* default_fragment_loc);

void engine_teardown();

/*
 * func is a call back which allows the user to insert code to be ran
 * into the main function
*/
void engine_process(void (*func)());

ShaderProgram* get_shaders();


/* ---- Sprite stuff ---- */
typedef int Sprite;
typedef GLuint Texture;

// Creation
Sprite sprite_make();
void sprite_free(Sprite sprite);
// Transform
void sprite_transform_set(Sprite sprite, mat4* new_transform);
void sprite_transform_get(Sprite sprite, mat4* rtrn_transform);
void sprite_transform_translate(Sprite sprite, vec3* pos);
void sprite_transform_rotate(Sprite sprite, float rot);
void sprite_transform_scale(Sprite sprite, vec3* scal);
// Texture
void sprite_texture_set(Sprite sprite, int ID);
Texture sprite_texture_get(Sprite sprite);

/* ---- Texture stuff ---- */
Texture texture_make(char* texture_location);
void texture_free(Texture tex);
