#include <stdlib.h>

#include "input.h"
#include "sprite.h"
#include "texture.h"
#include "window.h"

int main() {
	int err = InitWindow();
	if (err) {
		exit(-1);
	}
	err = InitSprites();
	if (err) {
		exit(-1);
	}

	TextureID tex0 = NewTexture("resources/textures/test.png");
	TextureID tex1 = NewTexture("resources/textures/test.png");

	int i = 0;
	while (!ShouldDestroyWindow()) {
		QueueSprite(-0.5, 0.5, 0.5, 1, 0.05*i, 1023, tex0);
		QueueSprite(0, 0, 1, 1, 0.05*i, 0, tex0);
		QueueSprite(0.5, 0.01*i, 1, 1, 0.02*i, 0, tex1);
		DrawSpriteQueue();
		ProcessWindow();

		if (IsKeyPressed('K')) {
			i++;
		}
	}

	DestroyWindow();

	return 0;
}
