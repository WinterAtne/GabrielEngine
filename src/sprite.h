#pragma once
#include <cglm/cglm.h>

#include "texture.h"

typedef struct {
	int ID;
	mat4 transform;
	Texture texture;
} Sprite;

// Initializing and Processing
void sprites_initialize(int window_x, int window_y, float window_scale);
void sprites_draw(); // Draws the sprite queue

// Creating
void sprite_make(Sprite** sprite); // Makes sprite, and adds it to the queue
void sprite_free(Sprite* sprite); // Frees sprite, and removes it from queue

// Modifying
void sprite_texture_set(Sprite* sprite, Texture texture);
void sprite_transform_translate(Sprite* sprite, vec3 translation);
