#include "shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

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
