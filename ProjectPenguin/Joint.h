#pragma once

#include "glm/glm.hpp"
#include <vector>

struct Joint
{
	int id;
	std::string name;
	std::vector<Joint*> children;

	glm::mat4 transform;	//Transformation from initial position relative to model, to position in the current pose
	glm::mat4 inverseInitialTransform;	//Inverse of the initial transformation of the joint relative to the model
};