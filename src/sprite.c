#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm//io.h>

#include <string.h>

#include "sprite.h"
#include "shader.h"

/* ---- Constants ---- */
const char* VERTEX_SHADER_LOCATION = "resources/shaders/default.vert";
const char* FRAGMENT_SHADER_LOCATION = "resources/shaders/default.frag";

/* ---- Variables ---- */
GLuint VAO, VBO, EBO;
ShaderProgram* shaders;
int modeLoc;

Sprite sprite_queue[16];

// This function just sets up an absurd amount of opengl boilerplate
void initialize_sprites(int window_x, int window_y, float window_scale) {
	/* ---- Shaders ---- */
	ShaderProgram* shaders = MakeShaderProgram(VERTEX_SHADER_LOCATION, FRAGMENT_SHADER_LOCATION);
	ShaderProgramActivate(shaders);

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

	int viewLoc = glGetUniformLocation(shaders->program, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, *view);

	float window_aspect = (float)window_x/(float)window_y;
	mat4 proj; glm_mat4_identity(proj);
	glm_ortho(
				-window_aspect * window_scale, window_aspect * window_scale, // x
				-window_scale, window_scale, // y
				 0.0f, 1.0f, // Z
				 proj); // Dest

	int projLoc = glGetUniformLocation(shaders->program, "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, *proj);


	/* TESTING STUFF */
	memset(sprite_queue, 0, 16 * sizeof(Sprite));
	modeLoc = glGetUniformLocation(shaders->program, "model");
}

/* ---- Private Functions ---- */
void render_sprite(Sprite* sprite) {
	glUniformMatrix4fv(modeLoc, 1, GL_FALSE, *sprite->transform);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

/* ---- Public Functions ---- */
void draw_sprites() {
	for (int i = 0; i < 16; i++) {
		if (sprite_queue[i].ID == 0) return;
		render_sprite(&sprite_queue[i]);
	}
}

void make_sprite(Sprite** sprite) {
	for (int i = 0; i < 16; i++) {
		if (sprite_queue[i].ID != 0) continue;

		*sprite = &sprite_queue[i];
		(*sprite)->ID = i+1;
		glm_mat4_identity((*sprite)->transform);
		return;
	}
	printf("FUCK\n");
}

void free_sprite(Sprite* sprite) {
	
}

void sprite_transform_translate(Sprite* sprite, vec3 translation) {
	glm_translate(sprite->transform, translation);
}
