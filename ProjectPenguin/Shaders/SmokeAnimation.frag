#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture0;

uniform float frameWidth;
uniform int currentFrame;

void main()
{
	vec2 newTexCoord = TexCoord;
	newTexCoord.x += frameWidth * float(currentFrame);

	vec4 color = texture(texture0, newTexCoord);
	if(color.w <= 0.0)
	{
		discard;
	}
    FragColor = color;
}