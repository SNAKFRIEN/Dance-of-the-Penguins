
#version 330 core
in vec4 position;

uniform vec3 lightPos;
uniform float farPlane;

void main()
{
	float lightDistance = length(position.xyz - lightPos);
	lightDistance = lightDistance / farPlane;
	gl_FragDepth = lightDistance;
}