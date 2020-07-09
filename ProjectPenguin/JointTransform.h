#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

struct JointTransform
{
	glm::vec3 position;
	glm::quat rotation;

	glm::mat4 GetLocalTransform()
	{
		glm::mat4 result(1.0f);
		result = glm::translate(result, position);
		return result * glm::mat4_cast(rotation);
	}

	static JointTransform Interpolate(const JointTransform& transA, const JointTransform& transB, float alpha)
	{
		JointTransform result;
		result.position = glm::mix(transA.position, transB.position, alpha);
		result.rotation = glm::mix(transA.rotation, transB.rotation, alpha);
		return result;
	}
};