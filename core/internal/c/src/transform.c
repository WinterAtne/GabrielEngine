#include "transform.h"

#include <string.h>
#include <cglm/cglm.h>

void transform_matrix(Transform* transform, mat4* matrix) {
	glm_mat4_identity(*matrix);
	glm_translated(*matrix, (vec3){transform->position_x, transform->position_y, transform->layer});
	glm_spinned(*matrix, transform->rotation, (vec3){0.0f, 0.0f, 1.0f});
	glm_scale(*matrix, (vec3){transform->scale_x, transform->scale_y, 1.0f});
}

void transform_zero(Transform* transform) {
	memset(transform, 0, sizeof(Transform));
	transform->scale_x = 1;
	transform->scale_y = 1;
}
