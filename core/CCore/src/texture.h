#pragma once

typedef struct {
	unsigned int id;
	int width, height;
} Texture;

extern Texture NewTexture(const char* location);
