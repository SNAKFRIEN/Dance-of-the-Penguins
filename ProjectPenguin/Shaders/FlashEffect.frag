#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture0;

uniform float brightness;

void main()
{
	FragColor = min(vec4(1.0), vec4(brightness) + texture(texture0, TexCoord));
}