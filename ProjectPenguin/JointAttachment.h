#pragma once

#include "Model.h"

class AnimatedModel;
class Camera;

class JointAttachment
{
public:
	JointAttachment(std::string name, const AnimatedModel& parentModel, std::string joint);
	void Draw(Camera& camera);
private:
	Model model;
	const glm::mat4& ownerModelTransform;
	const std::vector<glm::mat4>& pose;
	glm::mat4 transform;
	int id;
};
