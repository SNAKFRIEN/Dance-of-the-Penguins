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
float lightDist = length(lightPos - position);

float lightGradientRadius = 5.0;
const float ambient = 0.3;

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

float Smooth()
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

	float nDotL = dot(lightDir, normalize(normal));
	float lightIntensity = nDotL > 0 ? 1.0 : 0.7;

	return lightIntensity * clamp(distanceMultiplier, ambient, 1.0);
}

void main()
{
	FragColor = texture(tex, texcoord) * min(Smooth(), 1 - Shadow());
}