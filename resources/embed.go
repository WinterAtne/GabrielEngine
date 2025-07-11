package resources

import _ "embed"

// const DefaultSpriteVertexSource string = `
// #version 460 core
// layout (location = 0) in vec2 aPos;
// layout (location = 1) in vec2 aTexCoord;
//
// uniform mat4 model;
// uniform mat4 camera;
//
// out vec2 TexCoord;
//
// void main()
// {
// 	TexCoord = aTexCoord;
// 	gl_Position = camera * model * vec4(aPos, 1.0, 1.0);
// }`;
// const DefaultSpriteFragmentSource string = `
// #version 460 core
// in vec2 TexCoord;
//
// out vec4 FragColor;
//
// uniform sampler2D tex0;
//
// void main()
// {
// 	FragColor = texture(tex0, TexCoord);
// }`;

/* -- Shaders -- */
//go:embed shaders/default.vert
var DefaultSpriteVertexSource string
//go:embed shaders/default.vert
var DefaultSpriteFragmentSource string
