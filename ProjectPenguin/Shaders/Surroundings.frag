#version 330 core

const int MAX_LIGHTS = 10;

out vec4 FragColor;

in vec3 position;
in vec3 normal;
in vec2 texcoord;

uniform sampler2D tex;
uniform samplerCube shadowCubeMap;
uniform samplerCube shadowCubeMapBaked;

uniform float lightFarPlane;
uniform vec3 lightPos;

uniform vec3[MAX_LIGHTS] simpleLights;
uniform int nSimpleLights;

vec3 lightDir = normalize(lightPos - position);
float lightDist = length(lightPos - position);

float lightGradientRadius = 5.0;
const float ambient = 0.3;

const float simpleLightRadius = 3.0f;
const vec3 simpleLightColor = vec3(1.0, 0.8, 0.6);

//IceRink properties
const float right = 24.6;
const float top = 13.7;
const float cornerRadius = 5.5;

float Shadow()
{
	//Sample cube map
	vec3 fromLight = position - lightPos;
	float closestDepth = min(texture(shadowCubeMap, fromLight).r, texture(shadowCubeMapBaked, fromLight).r);
	closestDepth *= lightFarPlane;
	//Calculate current depth to compare
	float currrentDepth = length(fromLight);
	//Check if fragment is in Shadow
	float minBias = 0.005;
	float maxBias = 0.05;
	float bias = max(maxBias * (1.0 - dot(normal, lightDir)), minBias);
	return currrentDepth - bias > closestDepth ? 0.7 : 0.0;
}

vec3 Smooth()
{
	vec2 pos = abs(position.xz);
	float distanceMultiplier = 0.5;
	if(pos.x < right - cornerRadius)
	{
		distanceMultiplier = 1.0 - ((pos.y - top) / lightGradientRadius);
	}
	else if(pos.y < top - cornerRadius)
	{
		distanceMultiplier = 1.0 - ((pos.x - right) / lightGradientRadius);
	}
	else
	{
		float distanceToCorner = distance(pos.xy, vec2(right - cornerRadius, top - cornerRadius));
		distanceMultiplier = 1.0 - (distanceToCorner - cornerRadius) / lightGradientRadius;
	}

	//From simple light
	vec3 fromSimpleLights = vec3(0);
	for(int i = 0; i < nSimpleLights; i++)
	{
		fromSimpleLights += clamp((simpleLightRadius - distance(position, simpleLights[i])) / simpleLightRadius, 0.0, 1.0)
			* max(dot(simpleLights[i] - position, normal), 0.0)
			* simpleLightColor;
	}

	return vec3(1) * min(max(dot(lightDir, normal), 0.0) * clamp(distanceMultiplier, ambient, 1.0), 1 - Shadow()) + fromSimpleLights;
}

void main()
{
	vec4 textureColor = texture(tex, texcoord);
	if(all(lessThan(vec3(-0.001, 0.811, 0.949), textureColor.rgb)) && all(lessThan(textureColor.rgb, vec3(0.001, 0.812, 0.951))))
	{
		FragColor = vec4(1.0, 0.8, 0.5, 1.0);
	}
	else
	{
		FragColor = textureColor * vec4(Smooth(), 1);
	}
}