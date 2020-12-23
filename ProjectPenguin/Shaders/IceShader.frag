#version 330 core

const int MAX_COLLECTIBLES = 20;
const float MAX_COLLECTIBLE_HEIGHT = 10.0;

out vec4 FragColor;

in vec3 position;
in vec3 normal;
in vec2 texcoord;

uniform sampler2D tex;
uniform samplerCube shadowCubeMap;
uniform samplerCube shadowCubeMapBaked;

uniform float lightFarPlane;
uniform vec3 lightPos;

uniform int nCollectibles;
uniform vec3 collectibles[MAX_COLLECTIBLES];

vec3 lightDir = normalize(lightPos - position);

float Shadow()
{
	//Render collectible shadows
	for(int i = 0; i < nCollectibles; i++)
	{
		
		float radius = (MAX_COLLECTIBLE_HEIGHT - collectibles[i].y) / MAX_COLLECTIBLE_HEIGHT * 0.3;
		if(distance(position.xz, collectibles[i].xz) < radius)
		{
			
			return 0.3;
		}
	}	

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
	return currrentDepth - bias > closestDepth ? 0.3 : 0.0;
}

float Smooth()
{
	return max(0.3 * dot(lightDir, normal) + 0.7, 0.7);
}

void main()
{
	FragColor = texture(tex, texcoord) * min(Smooth(), 1 - Shadow());
}