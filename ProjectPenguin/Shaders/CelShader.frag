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

//REPLACE hard coded view pos
vec3 viewPos = vec3(0.0, 0.5, -1.0);

vec3 lightDir = normalize(lightPos - position);
vec3 viewDir = normalize(viewPos - position);

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
	float lightIntensity = nDotL > 0 ? 1.0 : 0.7;

	////Specular
	//vec3 reflectDir = reflect(lightDir, normalize(normal));
	//float specularIntensity = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	//specularIntensity = specularIntensity > 0.5 ? 1.0 : 0.0;
	//
	////REMOVE: Rim DONT FORGET THIS IS HERE (UNUSED)
	//float rimDot = nDotL * (1.0 - dot(-viewDir, normal));
	//rimDot = rimDot > 0.6 ? 1.0 : 0.0;

	return  lightIntensity;
}


//REPLACE: hardcoded shadow brightness values (0.7 and 0.3)
void main()
{
	FragColor = texture(tex, texcoord) * min(Cel(), 1 - Shadow());
}