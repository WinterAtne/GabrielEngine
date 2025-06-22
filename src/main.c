#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "engine.h"
#include "rendering.h"
#include "texture.h"


int main() {
	engine_init(
			800,
			600,
			1,
			"Hello World!",
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
	while(!engine_should_close()) {
		camera_transform_translate((vec3){0.022f, 0.01f, 0.0});
		engine_process();
	}

	// Tear Down
	engine_end();
	return 0;
}
