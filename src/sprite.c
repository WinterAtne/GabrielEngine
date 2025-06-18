#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm//io.h>

#include <string.h>

#include <stb_image.h>

#include "sprite.h"
#include "shader.h"
#include "utils.h"

/* ---- Constants ---- */
const char* VERTEX_SHADER_LOCATION = "resources/shaders/default.vert";
const char* FRAGMENT_SHADER_LOCATION = "resources/shaders/default.frag";

/* ---- Variables ---- */
GLuint VAO=0, VBO=0, EBO=0;
Shader shaders;
GLuint modeLoc, tex0Uni;
bool sprites_initialized = false;

Sprite* sprite_queue = NULL; // Note: [0] is allways null and never used
ulong sprite_queue_cap = 0;

// This function just sets up an absurd amount of opengl boilerplate
void sprites_initialize(int window_x, int window_y, float window_scale) {
	// Sprites should only be intialized once
	if (sprites_initialized) {
		error("Sprites already intialized");
		return;
	} else {
		sprites_initialized = true;
	}
	/* ---- Shaders ---- */
	shader_make(VERTEX_SHADER_LOCATION, FRAGMENT_SHADER_LOCATION, &shaders);
	shader_activate(&shaders);

	/* ---- Quad ---- */
	GLfloat vertices[] = {
		-0.5f,  0.5f,	0.0f, 0.0f, // Top Left
		 0.5,   0.5f,	1.0f, 0.0f, // Top Right
		-0.5f, -0.5f,	0.0f, 1.0f, // Bottom Left
		 0.5f, -0.5f,	1.0f, 1.0f, // Bottom Right
	};

	GLuint indices[] = {
		0, 1, 2,
		2, 1, 3,
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	/* ---- Render Matrix ---- */
	mat4 view; glm_mat4_identity(view);
	vec3 viewTranslation;
	float viewTranslationValues[] = {0.0f, 0.0f, 0.0f};
	glm_vec3_make(viewTranslationValues, viewTranslation);
	glm_translate(view, viewTranslation);

	int viewLoc = glGetUniformLocation(shaders.program, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, *view);

	float window_aspect = (float)window_x/(float)window_y;
	mat4 proj; glm_mat4_identity(proj);
	glm_ortho(
				-window_aspect * window_scale, window_aspect * window_scale, // x
				-window_scale, window_scale, // y
				 0.0f, 1.0f, // Z
				 proj); // Dest

	int projLoc = glGetUniformLocation(shaders.program, "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, *proj);


	/* TESTING STUFF */
	modeLoc = glGetUniformLocation(shaders.program, "model");

	tex0Uni = glGetUniformLocation(shaders.program, "tex0");
}

/* ---- Private Functions ---- */

// Assumes VAO is bound
void sprite_render(Sprite* sprite) {
	bind_texture(sprite->texture, tex0Uni);
	glUniformMatrix4fv(modeLoc, 1, GL_FALSE, *sprite->transform);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

/* ---- Public Functions ---- */
void sprites_draw() {
	assert(VAO != 0);
	glBindVertexArray(VAO);

	for (int i = 1; i < sprite_queue_cap; i++) {
		if (sprite_queue[i].ID == 0) continue;
		sprite_render(&sprite_queue[i]);
	}
}

Sprite* sprite_make() {
	for (int i = 1; i < sprite_queue_cap; i++) {
		if (sprite_queue[i].ID != 0) continue;

		sprite_queue[i].ID = i;
		glm_mat4_identity(sprite_queue[i].transform);
		return &sprite_queue[i];
	}

	int sprite_queue_cap_old = sprite_queue_cap;
	sprite_queue_cap += 16; // Would be better to multiply but that requires more initialization
	Sprite* new_sprite_queue = realloc(sprite_queue, sprite_queue_cap * sizeof(Sprite));
	if (new_sprite_queue == NULL) {
		error("Unable to allocate sprite");
		exit(-1); //What do you want me to do?
	}

	sprite_queue = new_sprite_queue;
	memset(sprite_queue + sprite_queue_cap_old, 0, sprite_queue_cap - sprite_queue_cap_old);
	return sprite_make(); // If it works its not a bad idea
}

void sprite_free(Sprite* sprite) {
	sprite->ID = 0;
}

void sprite_texture_set(Sprite* sprite, Texture texture) {
	sprite->texture = texture;
}

void sprite_transform_translate(Sprite* sprite, vec3 translation) {
	glm_translate(sprite->transform, translation);
}
