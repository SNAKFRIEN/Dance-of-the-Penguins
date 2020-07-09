#version 330 core
out vec4 FragColor;

in vec3 position;
in vec3 normal;
in vec2 texcoord;

uniform sampler2D tex;

vec3 lightPos = vec3(0, 10, 0);
vec3 lightDir = normalize(lightPos - position);
vec3 viewPos = vec3(0.0, 0.5, -1.0);
vec3 viewDir = normalize(viewPos - position);

float glossiness = 32;

vec4 Smooth()
{
	return texture(tex, texcoord) * max(dot(lightDir, normal), 0.0);
}

vec4 Cel()
{
	//Shadows
	float nDotL = dot(lightDir, normalize(normal));
	float lightIntensity = nDotL > 0 ? 1.0 : 0.2;

	//Specular
	vec3 reflectDir = reflect(lightDir, normalize(normal));
	float specularIntensity = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	specularIntensity = specularIntensity > 0.5 ? 1.0 : 0.0;

	//REMOVE: Rim DONT FORGET THIS IS HERE (UNUSED)
	float rimDot = nDotL * (1.0 - dot(-viewDir, normal));
	rimDot = rimDot > 0.6 ? 1.0 : 0.0;

	return texture(tex, texcoord) * (lightIntensity);
}

void main()
{
	FragColor = Cel();
}