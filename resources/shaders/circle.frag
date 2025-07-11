#version 460 core
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D tex0;

void main()
{
	if (pow(TexCoord.x - 0.5, 2) + pow(TexCoord.y - 0.5, 2) <= 0.25) {
		FragColor = texture(tex0, TexCoord);
	} else {
		discard;
	}
}
