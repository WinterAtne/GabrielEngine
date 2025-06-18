#pragma once

int engine_init(
		int window_x,
		int window_y,
		float window_scale,
		const char* window_name,
		const float clear_color[4]);
int engine_should_close();
void engine_process();
void engine_end();
