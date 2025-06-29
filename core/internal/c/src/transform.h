#pragma once

#include <cglm/types.h>

typedef struct {
	float position_x;
	float position_y;
	float scale_x;
	float scale_y;
	float rotation;
	float layer;
} Transform;

void transform_matrix(Transform* transform, mat4* matrix);
void transform_zero(Transform* transform);
