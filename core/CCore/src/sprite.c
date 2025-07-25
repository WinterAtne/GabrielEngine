#include "sprite.h"

#include <assert.h>
#include <cglm/cglm.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include "glfwwindow.h"
#include "errors.h"
#include "quad.h"
#include "shader.h"
#include "texture.h"
#include "windowopt.h"

typedef struct {
	mat4 transform;
	Shader* shader;
	Texture* texture;
} Sprite;

// Shaders
const char* DEFAULT_VERTEX_SHADER_FRAMEBUFFER_SOURCE = "\n\
#version 460 core\n\n\
layout (location = 0) in vec2 aPos;\n\
layout (location = 1) in vec2 aTexCoord;\n\
\n\
out vec2 TexCoord;\n\
\n\
void main()\n\
{\n\
	TexCoord = aTexCoord;\n\
	gl_Position = vec4(aPos, 0.0, 1.0);\n\
}";
const char* DEFAULT_FRAGMENT_SHADER_FRAMEBUFFER_SOURCE = "\
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
static Shader frameBufferShader = {};
static Quad spriteQuad = {};
static GLuint uCameraLocation = 0;
static GLuint uModelLocation = 0;
static GLuint uTextureLocation = 0;
static vec3 cameraPosition = {0, 0, 0};

static GLuint FBO = 0, RBO = 0;
static Quad frameBufferQuad = {};
static GLuint frameBufferTexture = 0;

static mat4 projectionMatrix = {};

static bool spritesInitialized = false;

/* ---- Private Functions ---- */

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


	/* - VAO, VBO, EBO | Sending data to GPU for quads - */
	spriteQuad = NewQuad(false);

	/* ---- FrameBuffer ---- */
	frameBufferQuad = NewQuad(true);
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &frameBufferTexture);
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, DEFAULT_VIRTUAL_WINDOW_LENGTH, DEFAULT_VIRTUAL_WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTexture, 0);

	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, DEFAULT_VIRTUAL_WINDOW_LENGTH, DEFAULT_VIRTUAL_WINDOW_LENGTH);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/* ---- Shader ---- */

	frameBufferShader = NewShader(DEFAULT_VERTEX_SHADER_FRAMEBUFFER_SOURCE, DEFAULT_FRAGMENT_SHADER_FRAMEBUFFER_SOURCE);
	if (!frameBufferShader.handle) return -1;
	
	float windowX = (float)DEFAULT_VIRTUAL_WINDOW_LENGTH/2;
	float windowY = (float)DEFAULT_VIRTUAL_WINDOW_HEIGHT/2;
	glm_mat4_identity(projectionMatrix);
	glm_ortho(
				-windowX, windowX, // x
				-windowY, windowY, // y
				-1024.0f, 0.0f, // Z
				 projectionMatrix); // Dest


	spriteCap = 1;
	spriteQueue = malloc(spriteCap * sizeof(Sprite));

	return 0;
}

extern void QueueSprite(
		float positionX, float positionY,
		float scaleX, float scaleY,
		float rotation,
		int layer,
		Texture* texture,
		Shader* shader) {
	assert(spritesInitialized);

	if (spriteCap == spriteEnd) {
		int err = expandQueue();
		if (err) {
			return;
		}
	}

	spriteQueue[spriteEnd].texture = texture;
	spriteQueue[spriteEnd].shader = shader;
	mat4* transform = &spriteQueue[spriteEnd].transform;
	glm_mat4_identity(*transform);
	glm_translated(*transform, (vec3){positionX, positionY, (float)layer});
	glm_spinned(*transform, rotation, (vec3){0.0f, 0.0f, 1.0f});
	glm_scale(*transform, (vec3){scaleX, scaleY, 1.0f});
	spriteEnd++;
}

extern void DrawSpriteQueue() {
	assert(spritesInitialized);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, DEFAULT_VIRTUAL_WINDOW_LENGTH, DEFAULT_VIRTUAL_WINDOW_HEIGHT);

	UseQuad(&spriteQuad);

	mat4 viewMatrix; glm_mat4_identity(viewMatrix);
	glm_translate(viewMatrix, cameraPosition);

	mat4 cameraMatrix;
	glm_mul(projectionMatrix, viewMatrix, cameraMatrix);

	for (int i = 0; i < spriteEnd; i++) {
		UseShader(spriteQueue[i].shader);
		uCameraLocation = glGetUniformLocation(spriteQueue[i].shader->handle, "camera");
		uModelLocation = glGetUniformLocation(spriteQueue[i].shader->handle, "model");
		uTextureLocation = glGetUniformLocation(spriteQueue[i].shader->handle, "tex0");
		glUniformMatrix4fv(uCameraLocation, 1, GL_FALSE, *cameraMatrix);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, spriteQueue[i].texture->id);
		glUniform1i(uTextureLocation, 0);


		glUniformMatrix4fv(uModelLocation, 1, GL_FALSE, *spriteQueue[i].transform);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, DEFAULT_WINDOW_LENGTH, DEFAULT_WINDOW_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT);

	UseShader(&frameBufferShader);
	UseQuad(&frameBufferQuad);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	spriteEnd = 0;
}

