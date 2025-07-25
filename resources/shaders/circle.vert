#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 camera;

out vec2 TexCoord;

void main()
{
	TexCoord = aTexCoord;
	gl_Position = camera * model * vec4(aPos, 1.0, 1.0);
}
