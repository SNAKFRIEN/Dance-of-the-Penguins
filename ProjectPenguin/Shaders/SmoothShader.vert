#version 330 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texcoord;

out vec3 position;
out vec3 normal;
out vec2 texcoord;

uniform mat4 model;
uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(in_position, 1);
	position = gl_Position.xyz;
	normal = normalize(mat3(model) * in_normal);
	position = vec3(model * vec4(in_position, 1.0));
	texcoord = in_texcoord;
}