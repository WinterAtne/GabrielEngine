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
static const char* VERTEX_SHADER_LOCATION = "resources/shaders/default.vert";
static const char* FRAGMENT_SHADER_LOCATION = "resources/shaders/default.frag";

static const int INVALID_SPRITE_ID = -1;
static const int INITIAL_SPRITE_CAP = 16;
static const float SPRITE_CAP_MULTIPLIER = 2.0f;

/* ---- Variables ---- */
static GLFWwindow* window;

static GLuint VAO=0, VBO=0, EBO=0;
static Shader default_shaders;
static GLuint model_uniform_location;
static GLuint camera_uniform_location;
static GLuint texture_unifrom_location;
static mat4 projMatrix, viewMatrix;

static InternalSprite* sprite_queue = NULL;
static int sprite_queue_cap = 0;

/* ---- Fundemental Functions ---- */

// TODO error callbacks could provide more information to user
static void glfw_error_callback(int error, const char* description) {
	error(description);
}

static void GLAPIENTRY glad_error_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	error(message);
}

// Mega OpenGL initialization function
int rendering_initialize(int window_x, int window_y, float window_scale, const char* window_name, const float clear_color[4]) {
	// Rendering should only be intialized once
	static bool rendering_initialized = false;
	if (rendering_initialized) {
		error("Rendering already initialized");
		return -1;
	} else {
		rendering_initialized = true;
	}

	/* ---- Begin OpenGL initialization --- */
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
	shader_make(VERTEX_SHADER_LOCATION, FRAGMENT_SHADER_LOCATION, &default_shaders);
	shader_activate(&default_shaders);

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

	/* ---- VAO, VBO, EBO | Sending data to GPU for quads --- */
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

	/* ---- Render Matricis & Uniforms ---- */
	float window_aspect = (float)window_x/(float)window_y;
	glm_mat4_identity(projMatrix);
	glm_ortho(
				-window_aspect * window_scale, window_aspect * window_scale, // x
				-window_scale, window_scale, // y
				 0.0f, 1.0f, // Z
				 projMatrix); // Dest

	glm_mat4_identity(viewMatrix);
	camera_uniform_location = glGetUniformLocation(default_shaders.program, "camera");

	model_uniform_location = glGetUniformLocation(default_shaders.program, "model");

	texture_unifrom_location = glGetUniformLocation(default_shaders.program, "tex0");

	return 0;
}

/* ---- Private Functions ---- */
static void texture_bind(Texture texture) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.handle);
	glUniform1i(texture_unifrom_location, 0);
}

// Assumes VAO is bound
static void sprite_render(InternalSprite* sprite) {
	texture_bind(sprite->texture);
	glUniformMatrix4fv(model_uniform_location, 1, GL_FALSE, *sprite->transform);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

static void sprites_draw() {
	assert(VAO != 0);
	glBindVertexArray(VAO);

	mat4 cameraMatrix;
	glm_mul(projMatrix, viewMatrix, cameraMatrix);
	glUniformMatrix4fv(camera_uniform_location, 1, GL_FALSE, *cameraMatrix);

	for (int i = 0; i < sprite_queue_cap; i++) {
		if (sprite_queue[i].ID == INVALID_SPRITE_ID) continue;
		sprite_render(&sprite_queue[i]);
	}
}

/* ---- Public Functions ---- */
bool window_should_close() {
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
