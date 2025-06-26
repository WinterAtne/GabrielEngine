#include "rendering.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm//io.h>
#include <stb_image.h>

#include <string.h>

#include "shader.h"
#include "utils.h"

/* ---- Constants ---- */
const char* VERTEX_SHADER_LOCATION = "resources/shaders/default.vert";
const char* FRAGMENT_SHADER_LOCATION = "resources/shaders/default.frag";

/* ---- Variables ---- */
GLuint VAO=0, VBO=0, EBO=0;
Shader shaders;
GLuint modeLoc, viewLoc, tex0Uni;

Sprite* sprite_queue = NULL; // Note: [0] is allways null and never used
mat4 viewMatrix;
ulong sprite_queue_cap = 1;

// This function just sets up an absurd amount of opengl boilerplate
void sprites_initialize(int window_x, int window_y, float window_scale) {
	static bool sprites_initialized = false;
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

	/* ---- Render Matricis ---- */
	float window_aspect = (float)window_x/(float)window_y;
	mat4 proj; glm_mat4_identity(proj);
	glm_ortho(
				-window_aspect * window_scale, window_aspect * window_scale, // x
				-window_scale, window_scale, // y
				 0.0f, 1.0f, // Z
				 proj); // Dest

	int projLoc = glGetUniformLocation(shaders.program, "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, *proj);

	glm_mat4_identity(viewMatrix);
	viewLoc = glGetUniformLocation(shaders.program, "view");

	modeLoc = glGetUniformLocation(shaders.program, "model");

	/* Textures */
	tex0Uni = glGetUniformLocation(shaders.program, "tex0");
}

/* ---- Private Functions ---- */
void texture_bind(Texture texture) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.handle);
	glUniform1i(tex0Uni, 0);
}

// Assumes VAO is bound
void sprite_render(Sprite* sprite) {
	texture_bind(sprite->texture);
	glUniformMatrix4fv(modeLoc, 1, GL_FALSE, *sprite->transform);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

/* ---- Public Functions ---- */
void camera_transform_translate(vec3 translation) {
	glm_vec3_negate(translation);
	glm_translated(viewMatrix, translation);
}

void sprites_draw() {
	assert(VAO != 0);
	glBindVertexArray(VAO);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, *viewMatrix);

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
	sprite_queue_cap *= 2;
	Sprite* new_sprite_queue = realloc(sprite_queue, sprite_queue_cap * sizeof(Sprite));
	if (new_sprite_queue == NULL) {
		error("Unable to allocate sprite");
		exit(-1); //What do you want me to do?
	}

	sprite_queue = new_sprite_queue;
	memset(sprite_queue + sprite_queue_cap_old, 0, (sprite_queue_cap - sprite_queue_cap_old) * sizeof(Sprite));
	return sprite_make(); // If it works its not a bad idea
}

void sprite_free(Sprite* sprite) {
	sprite->ID = 0;
}

void sprite_texture_set(Sprite* sprite, Texture texture) {
	sprite->texture = texture;
}

void sprite_transform_translate(Sprite* sprite, vec3 translation) {
	glm_translated(sprite->transform, translation);
}

void sprite_transform_rotate(Sprite *sprite, float rotation) {
	glm_spinned(sprite->transform, rotation, (vec3){0.0f, 0.0f, 1.0f});
}

void sprite_transform_scale(Sprite *sprite, vec3 scale) {
	glm_scale(sprite->transform, scale);
}

// Textures
Texture texture_make(const char* texture_location) {
	Texture texture;

	// Texture
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	unsigned char* data = stbi_load(texture_location, &width, &height, &nrChannels, 4);
	if (!data) {
		error("Could not load texture")
	} 

	glGenTextures(1, &texture.handle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.handle);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}
