
#include "quad.h"

#include <cglm/cglm.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

// Quad definitions.
static const GLfloat verticesFull[] = {
	-1.0f,  1.0f,	0.0f, 1.0f, // Top Left
	 1.0,   1.0f,	1.0f, 1.0f, // Top Right
	-1.0f, -1.0f,	0.0f, 0.0f, // Bottom Left
	 1.0f, -1.0f,	1.0f, 0.0f, // Bottom Right
};

static const GLfloat verticesHalf[] = {
	-0.5f,  0.5f,	0.0f, 0.0f, // Top Left
	 0.5,   0.5f,	1.0f, 0.0f, // Top Right
	-0.5f, -0.5f,	0.0f, 1.0f, // Bottom Left
	 0.5f, -0.5f,	1.0f, 1.0f, // Bottom Right
};

static const GLuint indices[] = {
	0, 2, 1,
	2, 3, 1,
};

Quad NewQuad(bool isFull) {
	Quad quad = {
		.VAO = 0,
		.VBO = 0,
		.EBO = 0,
	};

	/* ---- VAO, VBO, EBO | Sending data to GPU for quads --- */
	glGenVertexArrays(1, &quad.VAO);
	glGenBuffers(1, &quad.VBO);
	glGenBuffers(1, &quad.EBO);

	glBindVertexArray(quad.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, quad.VBO);
	if (isFull) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesFull), verticesFull, GL_STATIC_DRAW);
	} else {
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesHalf), verticesHalf, GL_STATIC_DRAW);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return quad;
}

void UseQuad(Quad* quad) {
	assert(quad != NULL && quad->VBO != 0 && quad->VAO != 0 && quad->EBO != 0);
	glBindVertexArray(quad->VAO);
}

void DestroyQuad(Quad* quad) {
	assert(quad != NULL && quad->VBO != 0 && quad->VAO != 0 && quad->EBO != 0);
	glDeleteVertexArrays(1, &quad->VAO);
	glDeleteBuffers(1, &quad->VBO);
	glDeleteBuffers(1, &quad->EBO);

	static const Quad new_quad = {
		.VAO = 0,
		.VBO = 0,
		.EBO = 0,
	};
	*quad = new_quad;
}

