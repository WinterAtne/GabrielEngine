#include "sprite.h"

#include <assert.h>
#include <cglm/cglm.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "texture.h"
#include "window_options.h"
#include "glfwwindow.h"

typedef struct {
	mat4 transform;
	TextureID texture;
} Sprite;

typedef struct {
	GLuint handle;
} Shader;

// Shaders
const char* DEFAULT_VERTEX_SHADER_SOURCE = "\n\
#version 460 core\n\n\
layout (location = 0) in vec2 aPos;\n\
layout (location = 1) in vec2 aTexCoord;\n\
\n\
uniform mat4 model;\n\
uniform mat4 camera;\n\
\n\
out vec2 TexCoord;\n\
\n\
void main()\n\
{\n\
	TexCoord = aTexCoord;\n\
	gl_Position = camera * model * vec4(aPos, 1.0, 1.0);\n\
}";
const char* DEFAULT_FRAGMENT_SHADER_SOURCE = "\
#version 460 core\n\
in vec2 TexCoord;\n\
\n\
out vec4 FragColor;\n\
\n\
uniform sampler2D tex0;\n\
\n\
void main()\n\
{\n\
	FragColor = texture(tex0, TexCoord);\n\
}";
static Sprite* spriteQueue = NULL;
static unsigned int spriteCap = 0;
static unsigned int spriteEnd = 0;
static float capMultiplier = 2.0;

// Rendering
static Shader defaultShader = {};
static GLuint VAO = 0, VBO = 0, EBO = 0;
static GLuint uCameraLocation = 0;
static GLuint uModelLocation = 0;
static GLuint uTextureLocation = 0;
static vec2 cameraPosition = {};

static mat4 projectionMatrix = {};

static bool spritesInitialized = false;

/* ---- Private Functions ---- */


Shader NewShader(const char* vertex, const char* fragment) {
	Shader shader;
	GLint success;

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertex, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLint maxLength = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar* log = malloc(maxLength * sizeof(GLchar));

		glGetShaderInfoLog(vertexShader, maxLength, NULL, log);
		printf("Vertex Shader Failed to Compile!\n OpenGL Error: %s", log);
		free(log);
		return (Shader){};
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragment, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLint maxLength = 0;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar* log = malloc(maxLength * sizeof(GLchar));

		glGetShaderInfoLog(fragmentShader, maxLength, NULL, log);
		printf("Fragment Shader Failed to Compile!\n OpenGL Error: %s", log);
		free(log);
		return (Shader){};
	}

	shader.handle = glCreateProgram();
	glAttachShader(shader.handle, vertexShader);
	glAttachShader(shader.handle, fragmentShader);
	glLinkProgram(shader.handle);
	glGetProgramiv(shader.handle, GL_LINK_STATUS, &success);
	if (!success) {
		GLint maxLength = 0;
		glGetProgramiv(shader.handle, GL_INFO_LOG_LENGTH, &maxLength);
		GLchar* log = malloc(maxLength * sizeof(GLchar));;

		glGetProgramInfoLog(shader.handle, maxLength, &maxLength, log);
		printf("Shader Failed to Link!\n OpenGL Error: %s", log);
		free(log);
		return (Shader){};
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shader;
}

void UseShader(Shader* shader) {
	glUseProgram(shader->handle);
}

extern int expandQueue() {
	assert(spritesInitialized);
	spriteCap *= capMultiplier;

	Sprite* new_spriteQueue = realloc(spriteQueue, spriteCap * sizeof(Sprite));
	if (new_spriteQueue == NULL) {
		error("Failled to expand sprite queue");
		return -1;
	} else {
		spriteQueue = new_spriteQueue;
		return 0;
	}
}

/* ---- Public Functions ---- */

extern int InitSprites() {
	assert(isWindowInitialized);
	if (spritesInitialized) {
		error("Sprites already initialized")
		return -1;
	} else {
		spritesInitialized = true;
	}

	static const GLfloat vertices[] = {
		-0.5f,  0.5f,	0.0f, 0.0f, // Top Left
		 0.5,   0.5f,	1.0f, 0.0f, // Top Right
		-0.5f, -0.5f,	0.0f, 1.0f, // Bottom Left
		 0.5f, -0.5f,	1.0f, 1.0f, // Bottom Right
	};
	
	static const GLuint indices[] = {
		0, 2, 1,
		2, 3, 1,
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


	defaultShader = NewShader(DEFAULT_VERTEX_SHADER_SOURCE, DEFAULT_FRAGMENT_SHADER_SOURCE);
	if (!defaultShader.handle) return -1;
	
	const float windowAspect = (float)DEFAULT_WINDOW_LENGTH/(float)DEFAULT_WINDOW_HEIGHT;
	glm_mat4_identity(projectionMatrix);
	glm_ortho(
				-windowAspect, windowAspect, // x
				-1, 1, // y
				-1024.0f, 0.0f, // Z
				 projectionMatrix); // Dest

	uCameraLocation = glGetUniformLocation(defaultShader.handle, "camera");
	uModelLocation = glGetUniformLocation(defaultShader.handle, "model");
	uTextureLocation = glGetUniformLocation(defaultShader.handle, "tex0");

	spriteCap = 1;
	spriteQueue = malloc(spriteCap * sizeof(Sprite));

	return 0;
}

extern void QueueSprite(
		float positionX, float positionY,
		float scaleX, float scaleY,
		float rotation,
		int layer,
		TextureID texture) {
	assert(spritesInitialized);

	if (spriteCap == spriteEnd) {
		int err = expandQueue();
		if (err) {
			return;
		}
	}

	spriteQueue[spriteEnd].texture = texture;
	mat4* transform = &spriteQueue[spriteEnd].transform;
	glm_mat4_identity(*transform);
	glm_translated(*transform, (vec3){positionX, positionY, (float)layer});
	glm_spinned(*transform, rotation, (vec3){0.0f, 0.0f, 1.0f});
	glm_scale(*transform, (vec3){scaleX, scaleY, 1.0f});
	spriteEnd++;
}

extern void DrawSpriteQueue() {
	assert(spritesInitialized);
	
	glBindVertexArray(VAO);
	UseShader(&defaultShader);

	mat4 viewMatrix; glm_mat4_identity(viewMatrix);
	glm_translate(viewMatrix, cameraPosition);

	mat4 cameraMatrix;
	glm_mul(projectionMatrix, viewMatrix, cameraMatrix);
	glUniformMatrix4fv(uCameraLocation, 1, GL_FALSE, *cameraMatrix);

	for (int i = 0; i < spriteEnd; i++) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, spriteQueue[i].texture);
		glUniform1i(uTextureLocation, 0);

		glUniformMatrix4fv(uModelLocation, 1, GL_FALSE, *spriteQueue[i].transform);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	spriteEnd = 0;
}

