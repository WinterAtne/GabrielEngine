#include "rendering.h"

int main() {
	rendering_initialize(800, 600, 1, "Hello World!", (float[4]){0.1f, 0.1f, 0.1f, 1.0f});

	Texture tex0 = texture_make("resources/textures/test_text.png");
	Sprite s0 = sprite_make();
	sprite_translate(s0, (vec2){-0.5f, -0.0f});
	sprite_set_layer(s0, 10.0f);
	sprite_texture_set(s0, tex0);

	Sprite s1 = sprite_make();
	sprite_translate(s1, (vec2){-1.2f, 0.5f});
	Texture tex1 = texture_make("resources/textures/test_text_1.jpg");
	sprite_texture_set(s1, tex1);

	// Main Loop
	char i = 0;
	while(!window_should_close()) {
		sprite_scale(s0, (vec2){0.999f, 1.0011f});
		sprite_rotate(s0, 0.001f);
		sprite_translate(s0, (vec2){0.0001f, -0.0001f});
		camera_transform_translate((vec2){0.0022f, 0.001f});

		Sprite s2 = sprite_make();
		sprite_translate(s2, (vec2){0.01f*i, 0.0002f*i*i});
		sprite_set_layer(s2, i);
		sprite_rotate(s2, 0.01f*i);
		sprite_scale(s2, (vec2){1.0f + 0.0001f*i*i, 1.0f});
		sprite_texture_set(s2, tex1);

		i++;
		rendering_process();
	}

	// Tear Down
	window_close();
	return 0;
}
