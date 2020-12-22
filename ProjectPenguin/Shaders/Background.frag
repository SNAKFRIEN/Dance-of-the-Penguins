#version 330 core
out vec4 FragColor;

in vec3 position;
in vec3 normal;
in vec2 texcoord;

uniform sampler2D tex;
//REMOVE?
//uniform samplerCube shadowCubeMap;
//uniform samplerCube shadowCubeMapBaked;

//uniform float lightFarPlane;
uniform vec3 lightPos;

vec3 lightDir = normalize(lightPos - position);

float Smooth()
{
	return max(dot(lightDir, normal), 0.0);
}

void main()
{
	FragColor = vec4(0.0, 0.1, 0.2, 1.0) * Smooth();
}