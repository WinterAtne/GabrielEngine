#include "rendering.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stb_image.h>

#include <string.h>

#include "shader.h"
#include "utils.h"

/* --- Types --- */

// Represents a sprite in memory containing all nessesary data to render.
// ID is its index in the sprite_queue
typedef struct {
	Sprite ID;
	float layer;
	vec2 position;
	vec2 scale;
	float rotation;
	Texture texture; // Probably make this a pointer
	// Color data
} InternalSprite;

typedef struct {
	GLuint VAO, VBO, EBO;
} Quad;

/* ---- Constants ---- */
static const char* VERTEX_SHADER_LOCATION = "resources/shaders/default.vert";
static const char* FRAGMENT_SHADER_LOCATION = "resources/shaders/default.frag";

static const int INVALID_SPRITE_ID = -1;
static const int INITIAL_SPRITE_CAP = 16;
static const float SPRITE_CAP_MULTIPLIER = 2.0f;
static const int BUFFER_SWAP_INTERVAL = 1;

// Quad
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

/* ---- Variables ---- */
static GLFWwindow* window;

static Shader default_shaders;
static GLuint model_uniform_location;
static GLuint camera_uniform_location;
static GLuint texture_unifrom_location;
static mat4 projMatrix;
static vec2 camera_position = {0.0f, 0.0f};
static Quad global_quad;

static InternalSprite* sprite_queue = NULL;
static int sprite_queue_cap = 0;


/* ---- Private Functions ---- */
static void glfw_error_callback(int error, const char* description) {
	error(description);
}

static void GLAPIENTRY glad_error_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	error(message);
}

Quad make_quad() {
	GLuint VAO=0, VBO=0, EBO=0;
	
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

	Quad quad;
	quad.VAO = VAO;
	quad.VBO = VBO;
	quad.EBO = EBO;

	return quad;
}

static void texture_bind(Texture texture) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.handle);
	glUniform1i(texture_unifrom_location, 0);
}

// Assumes VAO is bound
inline static void transform_make(InternalSprite* sprite, mat4 transform) {
	glm_mat4_identity(transform);
	glm_translated(transform, (vec3){sprite->position[0], sprite->position[1], sprite->layer});
	glm_spinned(transform, sprite->rotation, (vec3){0.0f, 0.0f, 1.0f});
	glm_scale(transform, (vec3){sprite->scale[0], sprite->scale[1], 1.0f});
}

static void sprite_render(InternalSprite* sprite) {
	texture_bind(sprite->texture);
	mat4 transform;
	transform_make(sprite, transform);
	glUniformMatrix4fv(model_uniform_location, 1, GL_FALSE, *transform);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

static void sprites_draw() {
	assert(global_quad.VAO != 0);
	glBindVertexArray(global_quad.VAO);

	mat4 viewMatrix; glm_mat4_identity(viewMatrix);
	glm_translate(viewMatrix, (vec3){camera_position[0], camera_position[1], 0.0f});

	mat4 cameraMatrix;
	glm_mul(projMatrix, viewMatrix, cameraMatrix);
	glUniformMatrix4fv(camera_uniform_location, 1, GL_FALSE, *cameraMatrix);

	shader_activate(&default_shaders);
	for (int i = 0; i < sprite_queue_cap; i++) {
		if (sprite_queue[i].ID == INVALID_SPRITE_ID) continue;
		sprite_render(&sprite_queue[i]);
	}
}

/* ---- Public Functions ---- */
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
	int err = 0;

	if (!glfwInit()) {
		error("glfw failled initialization")
		return -1;
	}
	glfwSetErrorCallback(glfw_error_callback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(window_x, window_y, window_name, NULL, NULL);
	if (window == NULL) {
		error("Failled to initialize glfw")
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(BUFFER_SWAP_INTERVAL); // Vsync = true
	gladLoadGL();
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(glad_error_callback, 0);
	glViewport(0, 0, window_x, window_y);
	glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	/* ---- Shaders ---- */
	const char* vertexShaderSource = file_read(VERTEX_SHADER_LOCATION);
	const char* fragmentShaderSource = file_read(FRAGMENT_SHADER_LOCATION);
	err = shader_make(vertexShaderSource, fragmentShaderSource, &default_shaders);
	free((char*)vertexShaderSource); vertexShaderSource = NULL;
	free((char*)fragmentShaderSource); fragmentShaderSource = NULL;
	if (err) { error("Shader initialization failled"); return err; }

	shader_activate(&default_shaders);

	/* ---- VAO ---- */
	global_quad = make_quad();

	/* ---- Render Matricis & Uniforms ---- */
	float window_aspect = (float)window_x/(float)window_y;
	glm_mat4_identity(projMatrix);
	glm_ortho(
				-window_aspect * window_scale, window_aspect * window_scale, // x
				-window_scale, window_scale, // y
				-1024.0f, 0.0f, // Z
				 projMatrix); // Dest

	camera_uniform_location = glGetUniformLocation(default_shaders.program, "camera");
	model_uniform_location = glGetUniformLocation(default_shaders.program, "model");
	texture_unifrom_location = glGetUniformLocation(default_shaders.program, "tex0");

	return 0;
}

bool window_should_close() {
	return glfwWindowShouldClose(window);
}

void window_close() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void rendering_process() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	sprites_draw();

	glfwPollEvents();
	glfwSwapBuffers(window);
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
		sprite_queue[i].position[0] = 0;
		sprite_queue[i].position[1] = 0;
		sprite_queue[i].scale[0] = 1;
		sprite_queue[i].scale[1] = 1;
		sprite_queue[i].rotation = 0;
		sprite_queue[i].layer = 0;
	}

	return sprite_make(); // If it works its not a bad idea
}

void sprite_free(Sprite sprite) {
	if (sprite >= sprite_queue_cap && sprite_queue[sprite].ID == sprite) {
		error("Attempted to modify out of bounds sprite");
	}
	sprite_queue[sprite].ID = INVALID_SPRITE_ID;
}

void sprite_texture_set(Sprite sprite, Texture texture) {
	if (sprite >= sprite_queue_cap && sprite_queue[sprite].ID == sprite) {
		error("Attempted to modify out of bounds sprite");
		return;
	}
	sprite_queue[sprite].texture = texture;
}

/* -- Mathy Stuff --- */
void camera_transform_translate(vec2 translation) {
	glm_vec2_negate(translation);
	camera_position[0] += translation[0];
	camera_position[1] += translation[1];
}

void sprite_translate(Sprite sprite, vec2 translation) {
	if (sprite >= sprite_queue_cap && sprite_queue[sprite].ID == sprite) {
		error("Attempted to modify out of bounds sprite");
		return;
	}
	sprite_queue[sprite].position[0] += translation[0];
	sprite_queue[sprite].position[1] += translation[1];
}

void sprite_set_layer(Sprite sprite, float layer) {
	if (sprite >= sprite_queue_cap && sprite_queue[sprite].ID == sprite) {
		error("Attempted to modify out of bounds sprite");
		return;
	}
	sprite_queue[sprite].layer = layer;
}

void sprite_rotate(Sprite sprite, float rotation) {
	if (sprite >= sprite_queue_cap && sprite_queue[sprite].ID == sprite) {
		error("Attempted to modify out of bounds sprite");
		return;
	}
	sprite_queue[sprite].rotation += rotation;
}

void sprite_scale(Sprite sprite, vec3 scale) {
	if (sprite >= sprite_queue_cap) {
		error("Attempted to modify out of bounds sprite");
		return;
	}
	sprite_queue[sprite].scale[0] *= scale[0];
	sprite_queue[sprite].scale[1] *= scale[1];
}

/* -- Textures -- */
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
