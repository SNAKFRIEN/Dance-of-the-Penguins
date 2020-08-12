#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform vec2 pos;
uniform vec2 scale;

void main()
{
	vec2 realPos = aPos;
	realPos *= scale;
	realPos += pos;
    gl_Position = vec4(realPos, 0.0, 1.0);
    TexCoord = aTexCoord;
}