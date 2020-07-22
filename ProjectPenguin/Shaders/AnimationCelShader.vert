#version 330 core

const int MAX_JOINTS = 200;
const int MAX_WEIGHTS = 4;

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texcoord;
layout (location = 3) in ivec4 in_jointIndices;
layout (location = 4) in vec4 in_weights;

out vec3 position;
out vec3 normal;
out vec2 texcoord;
//REMOVE
out ivec4 jointTransformIds;

uniform mat4 jointTransforms[MAX_JOINTS];
uniform mat4 model;
uniform mat4 mvp;

void main()
{
	jointTransformIds = in_jointIndices;

	//Animation is applied by taking weighted average of joints that affect this vertex

	//Local position after animation has been applied
	vec4 totalLocalPos = vec4(0.0);
	//Normal after animation has been applied
	vec4 totalNormal = vec4(0.0);
	
	//Loop through weights to apply animation
	for(int i = 0; i < MAX_WEIGHTS; i++)
	{
		vec4 localPosition = jointTransforms[in_jointIndices[i]] * vec4(in_position, 1.0);
		totalLocalPos += localPosition * in_weights[i];

		vec4 worldNormal = jointTransforms[in_jointIndices[i]] * vec4(in_normal, 0.0);
		totalNormal += worldNormal * in_weights[i];
	}

	gl_Position = mvp * totalLocalPos;
	normal = (model * totalNormal).xyz;
	position = vec3(model * totalLocalPos);
	texcoord = in_texcoord;
}