#pragma once
#include "shader.h"
#include "texture.h"

extern int InitSprites();

// Queues a sprite to be drawn using the transform and texture.
extern void QueueSprite(
		float positionX, float positionY,
		float scaleX, float scaleY,
		float rotation,
		int layer,
		Texture* texture,
		Shader* shader);

extern void DrawSpriteQueue();
