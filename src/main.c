#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>

#include "engine.h"
#include "sprite.h"
#include "texture.h"

const int WINDOW_SIZE_X = 1920;
const int WINDOW_SIZE_Y = 1080;
const float WINDOW_ASPECT = (float)WINDOW_SIZE_X/(float)WINDOW_SIZE_Y;
const float WINDOW_SCALE = 1;
const char* WINDOW_NAME = "Hello World!";

int main() {
	engine_init(
			WINDOW_SIZE_X,
			WINDOW_SIZE_Y,
			WINDOW_SCALE,
			WINDOW_NAME,
			(const float[4]){0.1f, 0.1f, 0.1f, 1.0f}
			);

	Texture tex0 = make_texture("resources/textures/test_text.png");
	Sprite* s0 = sprite_make();
	sprite_transform_translate(s0, (vec3){0.1f, 0.1f, 0.0f});
	sprite_texture_set(s0, tex0);

	Sprite* s1 = sprite_make();
	sprite_transform_translate(s1, (vec3){-1.0f, 0.5f, 0.0f});
	Texture tex1 = make_texture("resources/textures/test_text_1.jpg");
	sprite_texture_set(s1, tex1);


	// Main Loop
	char i = 0;
	while(!engine_should_close()) {
		Sprite* s2 = sprite_make();
		sprite_transform_translate(s2, (vec3){0.1f*i, 0.1f*i, 0.0f});
		sprite_texture_set(s2, tex0);

		Sprite* s3 = sprite_make();
		sprite_transform_translate(s3, (vec3){-0.1f*i*((float)i/2), 0.1f*i, 0.0f});
		sprite_texture_set(s3, tex1);

		Sprite* s4 = sprite_make();
		sprite_transform_translate(s4, (vec3){0.1f*i*((float)i/2), -0.1f*i*((float)i/2), 0.0f});
		sprite_texture_set(s4, tex0);

		if (i == 64) {
			sprite_free(s0);
		} else if (i == 127) {
			s0 = sprite_make();
			sprite_transform_translate(s0, (vec3){0.1f, 0.1f, 0.0f});
			sprite_texture_set(s0, tex0);
		}

		i++;
		engine_process();
	}

	// Tear Down
	engine_end();
	return 0;
}
