#pragma once

#include "Model.h"

class AnimatedModel;
class Camera;

class JointAttachment
{
public:
	JointAttachment(std::string name, const AnimatedModel& parentModel, std::string joint, std::string vertShader = "CelShader.vert", std::string fragShader = "CelShader.frag");
	JointAttachment(const JointAttachment& rhs);
	JointAttachment operator=(const JointAttachment& rhs);
	JointAttachment(JointAttachment&& rhs) noexcept;
	JointAttachment operator=(JointAttachment&& rhs) = delete;

	void Draw(Camera& camera);
private:
	const std::string name;
	const std::string jointName;

	Model model;
	const glm::mat4& ownerModelTransform;
	const std::vector<glm::mat4>& pose;
	glm::mat4 transform;
	int id;
};
