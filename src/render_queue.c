#include "render_queue.h"

#include <stdio.h>
#include <string.h>
#include <stb_image.h>
#include <cglm/io.h>

#include "shader.h"
#include "utils.h"


GLFWwindow* window;
ShaderProgram* shaders;
Sprite* sprite_queue;
mat4* sprite_transform_queue;
Texture* sprite_texture_queue;
uint sprite_queue_end = 0; // Last element of sprite_queue
uint sprite_queue_cap = 0; // Capacity of sprite_queue

Texture* textures_list;
uint textures_list_end = 0; // Last element of sprite_queue
uint textures_list_cap = 0; // Capacity of sprite_queue

void engine_setup(
		int window_size_x,
		int window_size_y,
		int window_scale,
		char* window_name,
		char* default_vertex_loc,
		char* default_fragment_loc) {
	// Setup
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(window_size_x, window_size_y, window_name, NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failled to create Window\n");
		glfwTerminate();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, window_size_x, window_size_y);
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

	// Shaders
	shaders = MakeShaderProgram(default_vertex_loc, default_fragment_loc);
	ShaderProgramActivate(shaders);

	mat4 view; glm_mat4_identity(view);
	vec3 viewTranslation;
	float viewTranslationValues[] = {0.0f, 0.0f, 0.0f};
	glm_vec3_make(viewTranslationValues, viewTranslation);
	glm_translate(view, viewTranslation);

	float window_aspect = (float)window_size_x/(float)window_size_y;
	mat4 proj; glm_mat4_identity(proj);
	glm_ortho(
				-window_aspect * window_scale, window_aspect * window_scale, // X
				-window_scale, window_scale, // Y
				 0.0f, 1.0f, // Z
				 proj); // Dest
	int viewLoc = glGetUniformLocation(get_shaders()->program, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, *view);

	int projLoc = glGetUniformLocation(get_shaders()->program, "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, *proj);

	sprite_queue_cap = 16;
	sprite_queue = malloc(sprite_queue_cap * sizeof(Sprite));
	sprite_queue_cap = 16;
	sprite_transform_queue = malloc(sprite_queue_cap * sizeof(mat4));

	textures_list_cap = 16;
	textures_list = malloc(textures_list_cap * sizeof(Texture));
}

void engine_teardown() {
	free(sprite_queue);
	free(textures_list);
	ShaderProgramDelete(shaders);
}

void engine_process(void (*func)()) {
	if (!func) {
		ERROR("main function is null");
		exit(-1);
	}

	// Setup Triangle
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
	
	GLuint VAO, VBO, EBO;

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

	Sprite sprite = sprite_make();
	mat4 trans; glm_mat4_identity(trans);
	sprite_transform_set(sprite, &trans);
	sprite_transform_translate(sprite, &(vec3){1.0f, 1.0f, 0.0f});

	sprite = sprite_make();
	sprite_transform_set(sprite, &trans);
	sprite_transform_translate(sprite, &(vec3){-0.5f, 0.5f, 0.0f});

	int modelLoc = glGetUniformLocation(get_shaders()->program, "model");
	glUniformMatrix4fv(modelLoc, sprite_queue_end, GL_FALSE, **sprite_transform_queue);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glfwSwapBuffers(window);
		glClear(GL_COLOR_BUFFER_BIT);

		func();

		sprite_transform_translate(sprite, &(vec3){-0.01f, 0.0f, 0.0f});
		int modelLoc = glGetUniformLocation(get_shaders()->program, "model");
		glUniformMatrix4fv(modelLoc, sprite_queue_end, GL_FALSE, **sprite_transform_queue);

		GLenum code;
		code = glGetError();
		if (code != GL_NO_ERROR) {
			ERROR("something wrong\n");
			exit(-1);
		}
		
		glBindVertexArray(VAO);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 2);

	}
}

ShaderProgram* get_shaders() {
	return shaders;
}

/* ---- Sprite stuff ---- */

// Creation
Sprite sprite_make() {
	if (sprite_queue_end+2 == sprite_queue_cap) {
		sprite_queue_cap *= 2;
		Sprite* new_sprite_queue = realloc(sprite_queue, sprite_queue_cap * sizeof(Sprite));
		mat4* new_transform_queue = realloc(sprite_transform_queue, sprite_queue_cap * sizeof(Sprite));
		if (new_sprite_queue || new_transform_queue) {
			ERROR("Sprite allocation failled");
			exit(-1);
		}

		sprite_queue = new_sprite_queue;
		sprite_transform_queue = new_transform_queue;
	}
	
	sprite_queue[sprite_queue_end] = sprite_queue_end;
	return sprite_queue_end++;
}

void sprite_free(Sprite sprite) {
	sprite_queue[sprite] = 0;
}

// Transform
void sprite_transform_set(Sprite sprite, mat4* new_transform) {
	// memcpy(sprite_transform_queue[sprite], new_transform, sizeof(mat4));
	glm_mat4_copy(*new_transform, sprite_transform_queue[sprite]);
}

void sprite_transform_get(Sprite sprite, mat4* rtrn_transform) {
	memcpy(rtrn_transform, sprite_transform_queue[sprite], sizeof(mat4));
}

void sprite_transform_translate(Sprite sprite, vec3* pos) {
	glm_translate(sprite_transform_queue[sprite], *pos);
}

void sprite_transform_rotate(Sprite sprite, float rot) {
	vec3 axis = (float[3]){0.0f, 0.0f, 1.0f};
	glm_rotate(sprite_transform_queue[sprite], rot, axis);
}

void sprite_transform_scale(Sprite sprite, vec3* scale) {
	glm_scale(sprite_transform_queue[sprite], *scale);
}

// Texture
void sprite_texture_set(Sprite sprite, int ID) {
	sprite_texture_queue[sprite] = ID;
}

Texture sprite_texture_get(Sprite sprite) {
	return sprite_texture_queue[sprite];
}

/* ---- Texture stuff ---- */
Texture texture_make(char* texture_location) {
	if (textures_list_end+2 == textures_list_cap) {
		textures_list_cap *= 2;
		Texture* new_textures_list = realloc(textures_list, sprite_queue_cap * sizeof(Sprite));
		if (new_textures_list) {
			ERROR("Sprite allocation failled");
		}

		textures_list = new_textures_list;
	}
	
	textures_list_end++;
	// Texture
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	unsigned char* data = stbi_load("resources/textures/test_text.png", &width, &height, &nrChannels, 0);
	if (!data) {
		printf("no textures :(\n");
		exit(-1);
	} 

	Texture texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	textures_list[textures_list_cap] = texture;

	return textures_list_end;
}

void texture_free(Texture tex) {
	
}
