#include "rendering.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm//io.h>
#include <stb_image.h>

#include <string.h>

#include "shader.h"
#include "utils.h"

/* --- Types --- */

// Represents a sprite in memory containing all nessesary data to render.
// ID is its index in the sprite_queue
typedef struct {
	Sprite ID;
	mat4 transform;  // Probably make this a pointer
	Texture texture; // Probably make this a pointer
	// Color data
	// Layer
} InternalSprite;

/* ---- Constants ---- */
const char* VERTEX_SHADER_LOCATION = "resources/shaders/default.vert";
const char* FRAGMENT_SHADER_LOCATION = "resources/shaders/default.frag";

const int INVALID_SPRITE_ID = -1;
const int INITIAL_SPRITE_CAP = 16;
const float SPRITE_CAP_MULTIPLIER = 2.0f;

/* ---- Variables ---- */
GLFWwindow* window;

GLuint VAO=0, VBO=0, EBO=0;
Shader shaders;
GLuint modeLoc, viewLoc, tex0Uni;
mat4 viewMatrix;

InternalSprite* sprite_queue = NULL;
int sprite_queue_cap = 0;

/* ---- Fundemental Functions ---- */
void glfw_error_callback(int error, const char* description) {
	error(description);
}

void GLAPIENTRY glad_error_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	error(message);
}

int rendering_initialize(int window_x, int window_y, float window_scale, const char* window_name, const float clear_color[4]) {
	static bool rendering_initialized = false;
	// Sprites should only be intialized once
	if (rendering_initialized) {
		error("Rendering already initialized");
		return -1;
	} else {
		rendering_initialized = true;
	}

	if (!glfwInit()) {
		error("glfw failled initialization")
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwSwapInterval(1); // Vsync = true

	window = glfwCreateWindow(window_x, window_y, window_name, NULL, NULL);
	if (window == NULL) {
		error("Failled to initialize glfw")
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetErrorCallback(glfw_error_callback);
	gladLoadGL();
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(glad_error_callback, 0);
	glViewport(0, 0, window_x, window_y);
	glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);

	/* ---- Shaders ---- */
	shader_make(VERTEX_SHADER_LOCATION, FRAGMENT_SHADER_LOCATION, &shaders);
	shader_activate(&shaders);

	/* ---- Quad ---- */
	static const GLfloat vertices[] = {
		-0.5f,  0.5f,	0.0f, 0.0f, // Top Left
		 0.5,   0.5f,	1.0f, 0.0f, // Top Right
		-0.5f, -0.5f,	0.0f, 1.0f, // Bottom Left
		 0.5f, -0.5f,	1.0f, 1.0f, // Bottom Right
	};

	static const GLuint indices[] = {
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

	return -1;
}

/* ---- Private Functions ---- */
void texture_bind(Texture texture) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.handle);
	glUniform1i(tex0Uni, 0);
}

// Assumes VAO is bound
void sprite_render(InternalSprite* sprite) {
	texture_bind(sprite->texture);
	glUniformMatrix4fv(modeLoc, 1, GL_FALSE, *sprite->transform);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void sprites_draw() {
	assert(VAO != 0);
	glBindVertexArray(VAO);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, *viewMatrix);

	for (int i = 0; i < sprite_queue_cap; i++) {
		if (sprite_queue[i].ID == INVALID_SPRITE_ID) continue;
		sprite_render(&sprite_queue[i]);
	}
}

/* ---- Public Functions ---- */
int window_should_close() {
	return glfwWindowShouldClose(window);
}

void window_close() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void rendering_process() {
	glClear(GL_COLOR_BUFFER_BIT);

	sprites_draw();

	glfwPollEvents();
	glfwSwapBuffers(window);
}

void camera_transform_translate(vec3 translation) {
	glm_vec3_negate(translation);
	glm_translated(viewMatrix, translation);
}

Sprite sprite_make() {
	for (int i = 0; i < sprite_queue_cap; i++) {
		if (sprite_queue[i].ID != INVALID_SPRITE_ID) continue;

		sprite_queue[i].ID = i;
		return sprite_queue[i].ID;
	}

	int sprite_queue_cap_old = sprite_queue_cap;
	if (sprite_queue_cap > 0) sprite_queue_cap *= SPRITE_CAP_MULTIPLIER;
	else sprite_queue_cap = INITIAL_SPRITE_CAP;

	InternalSprite* new_sprite_queue = realloc(sprite_queue, sprite_queue_cap * sizeof(InternalSprite));
	if (new_sprite_queue == NULL) {
		error("Unable to allocate sprite");
		exit(-1); //What do you want me to do?
	}

	sprite_queue = new_sprite_queue;

	for (int i = sprite_queue_cap_old; i < sprite_queue_cap; i++) {
		sprite_queue[i].ID = INVALID_SPRITE_ID;
		glm_mat4_identity(sprite_queue[i].transform);
	}

	return sprite_make(); // If it works its not a bad idea
}

void sprite_free(Sprite sprite) {
	if (sprite >= sprite_queue_cap) {
		error("Attempted to modify out of bounds sprite");
	}
	sprite_queue[sprite].ID = INVALID_SPRITE_ID;
}

void sprite_texture_set(Sprite sprite, Texture texture) {
	if (sprite >= sprite_queue_cap) {
		error("Attempted to modify out of bounds sprite");
	}
	sprite_queue[sprite].texture = texture;
}

void sprite_transform_translate(Sprite sprite, vec3 translation) {
	if (sprite >= sprite_queue_cap) {
		error("Attempted to modify out of bounds sprite");
	}
	glm_translated(sprite_queue[sprite].transform, translation);
}

void sprite_transform_rotate(Sprite sprite, float rotation) {
	if (sprite >= sprite_queue_cap) {
		error("Attempted to modify out of bounds sprite");
	}
	glm_spinned(sprite_queue[sprite].transform, rotation, (vec3){0.0f, 0.0f, 1.0f});
}

void sprite_transform_scale(Sprite sprite, vec3 scale) {
	if (sprite >= sprite_queue_cap) {
		error("Attempted to modify out of bounds sprite");
	}
	glm_scale(sprite_queue[sprite].transform, scale);
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
