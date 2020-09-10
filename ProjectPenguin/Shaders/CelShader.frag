#version 330 core
out vec4 FragColor;

in vec3 position;
in vec3 normal;
in vec2 texcoord;
in vec4 lightSpacePosition;

uniform sampler2D tex;
uniform sampler2D shadowMap;

//REPLACE hard coded view pos and light pos
vec3 lightPos = vec3(0, 10, 0);
vec3 lightDir = normalize(lightPos - position);
vec3 viewPos = vec3(0.0, 0.5, -1.0);
vec3 viewDir = normalize(viewPos - position);

float glossiness = 32;

vec4 Smooth()
{
	return texture(tex, texcoord) * max(dot(lightDir, normal), 0.0);
}

float Shadow()
{
	//Perspective divide
	vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
	//transform NDC to range [0, 1]
	projCoords = projCoords * 0.5f + 0.5f;
	//Sample shadow texture
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	//Check if fragment is in Shadow
	float minBias = 0.00005;
	float maxBias = 0.0005;
	float bias = max(maxBias * (1.0 - dot(normal, lightDir)), minBias);
	return projCoords.z - bias > closestDepth ? 0.3 : 0.0;
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