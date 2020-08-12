#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture0;

uniform int value;

void main()
{
	vec2 newTexCoord = TexCoord;
	newTexCoord.x += 0.1 * value;

    FragColor = texture(texture0, newTexCoord);
}