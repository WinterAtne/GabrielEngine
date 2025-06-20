#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "engine.h"
#include "rendering.h"
#include "texture.h"

const int WINDOW_SIZE_X = 800;
const int WINDOW_SIZE_Y = 600;
const float WINDOW_ASPECT = (float)WINDOW_SIZE_X/(float)WINDOW_SIZE_Y;
const float WINDOW_SCALE = 1;
char* WINDOW_NAME = "Hello World!";

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
	sprite_texture_set(s0, tex0);

	Sprite* s1 = sprite_make();
	sprite_transform_translate(s1, (vec3){-1.0f, 0.5f, 0.0f});
	Texture tex1 = make_texture("resources/textures/test_text_1.jpg");
	sprite_texture_set(s1, tex1);

	// Main Loop
	int i = 0;
	while(!engine_should_close()) {
		sprite_transform_scale(s0, (vec3){0.9999f, 1.01111f, 1.0f});
		sprite_transform_rotate(s0, -0.01f);
		sprite_transform_translate(s0, (vec3){0.001f, 0.001f, 0.0f});
		i++;
		engine_process();
	}

	// Tear Down
	engine_end();
	return 0;
}
