#pragma once

// Represents a shader in memory.
typedef struct {
	unsigned int handle;
} Shader;


// Creates a new shader.
Shader NewShader(const char* vertex, const char* fragment);
// Activates a shader.
void UseShader(Shader* shader);
