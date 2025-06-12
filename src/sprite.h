#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stb_image.h>

#include <stdio.h>

#include "shader.h"

typedef struct {
	uint ID;
	mat4 transform;
} Sprite;

void draw_sprites(); // Draws the sprite queue
void make_sprite(Sprite* sprite, mat4 transform); // Makes sprite, and adds it to the queue
void free_sprite(Sprite* sprite); // Frees sprite, and removes it from queue
