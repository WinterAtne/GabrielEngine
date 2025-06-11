#version 330 core
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D tex0;

void main()
{
	// FragColor = texture(tex0, TexCoord);
	FragColor = mix(texture(tex0, TexCoord), vec4(0.5, 0.5, 0.5, 1.0), 0.2);
}


