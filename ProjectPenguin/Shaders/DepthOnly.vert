#version 330 core
layout (location = 0) in vec3 in_position;

uniform mat4 modelTransform;

void main()
{
	gl_Position = modelTransform * vec4(in_position, 1.0);
}  