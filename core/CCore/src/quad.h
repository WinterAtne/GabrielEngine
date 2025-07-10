#pragma once

#include <stdbool.h>

// Represents a 2d square in memory.
typedef struct {
	unsigned int VAO, VBO, EBO;
} Quad;

Quad NewQuad(bool isFull);
void UseQuad(Quad* quad);

// Frees the gpu memory assosiated with quad and sets its elements to 0.
void DestroyQuad(Quad* quad);
