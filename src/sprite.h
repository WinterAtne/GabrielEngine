#pragma once
#include <cglm/cglm.h>

#include "texture.h"

typedef struct {
	int ID;
	mat4 transform;
	Texture texture;
} Sprite;

// Initializing and Processing
void initialize_sprites(int window_x, int window_y, float window_scale);
void draw_sprites(); // Draws the sprite queue

// Creating
void make_sprite(Sprite** sprite); // Makes sprite, and adds it to the queue
void free_sprite(Sprite* sprite); // Frees sprite, and removes it from queue

// Modifying
void sprite_texture_set(Sprite* sprite, Texture texture);
void sprite_transform_translate(Sprite* sprite, vec3 translation);
