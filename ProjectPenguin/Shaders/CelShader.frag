#version 330 core
out vec4 FragColor;

in vec3 position;
in vec3 normal;
in vec2 texcoord;

uniform sampler2D tex;
uniform samplerCube shadowCubeMap;
uniform samplerCube shadowCubeMapBaked;

uniform float lightFarPlane;
uniform vec3 lightPos;

vec3 lightDir = normalize(lightPos - position);

float glossiness = 32.0;

vec4 Smooth()
{
	return texture(tex, texcoord) * max(dot(lightDir, normal), 0.0);
}

float Shadow()
{
	//Sample cube map
	vec3 fromLight = position - lightPos;
	float closestDepth = min(texture(shadowCubeMap, fromLight).r, texture(shadowCubeMapBaked, fromLight).r);
	closestDepth *= lightFarPlane;
	//Calculate current depth to compare
	float currentDepth = length(fromLight);
	//Check if fragment is in Shadow
	float minBias = 0.005;
	float maxBias = 0.05;
	float bias = max(maxBias * (1.0 - dot(normal, lightDir)), minBias);
	return currentDepth - bias > closestDepth ? 0.3 : 0.0;
}


float Cel()
{
	//Cel shading
	float nDotL = dot(lightDir, normalize(normal));
	return nDotL > 0 ? 1.0 : 0.7;
}


//REPLACE: hardcoded shadow brightness values (0.7 and 0.3)
void main()
{
	FragColor = texture(tex, texcoord) * min(Cel(), 1 - Shadow());
}