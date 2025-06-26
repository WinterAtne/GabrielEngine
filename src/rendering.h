#pragma once
#include <glad/glad.h>
#include <cglm/cglm.h>

typedef struct {
	GLuint handle;
} Texture;

typedef struct {
	int ID;
	mat4 transform;
	Texture texture;
} Sprite;

// Initializing and Processing
void sprites_initialize(int window_x, int window_y, float window_scale);
void sprites_draw(); // Draws the sprite queue

// Creating
Texture texture_make(const char* texture_location);
Sprite* sprite_make(); // Makes sprite, and adds it to the queue
void sprite_free(Sprite* sprite); // Frees sprite, and removes it from queue

// Camera
void camera_transform_translate(vec3 translation);

// Modifying
void sprite_texture_set(Sprite* sprite, Texture texture);
void sprite_transform_translate(Sprite* sprite, vec3 translation);
void sprite_transform_rotate(Sprite* sprite, float rotation);
void sprite_transform_scale(Sprite* sprite, vec3 scale);
