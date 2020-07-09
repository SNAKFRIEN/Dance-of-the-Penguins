#pragma once

#include "glm/glm.hpp"
#include <vector>

struct Joint
{
	int id;
	std::string name;
	std::vector<Joint*> children;

	glm::mat4 transform;	//Transformation from initial position relative to model, to position in the current pose
	//REMOVE: initialLocalTransform is not actually required (it would only be used for inverseInitialTransform
	//glm::mat4 initialLocalTransform;	//Initial transformation of the joint relative to its parent
	glm::mat4 inverseInitialTransform;	//Inverse of the initial transformation of the joint relative to the model
};