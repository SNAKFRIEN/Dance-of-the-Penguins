#pragma once

#include "AnimatedModel.h"

class Camera;

class AnimatedJointAttachment
{
public:
	AnimatedJointAttachment(std::string name, const AnimatedModel& parentModel, std::string joint, std::string vertShader = "CelShader.vert", std::string fragShader = "CelShader.frag");
	AnimatedJointAttachment(const AnimatedJointAttachment& rhs);
	AnimatedJointAttachment operator=(const AnimatedJointAttachment& rhs);
	AnimatedJointAttachment(AnimatedJointAttachment&& rhs) noexcept;
	AnimatedJointAttachment operator=(AnimatedJointAttachment&& rhs) = delete;

	void UpdateAnimation(float dt);
	void Draw(Camera& camera);
	
	void SetAnimation(std::string name);

	const AnimatedModel& GetModel() const;
private:
	const std::string name;
	const std::string jointName;

	AnimatedModel model;	//This is the model that will be attached to a joint in the base model
	const glm::mat4& ownerModelTransform;
	const std::vector<glm::mat4>& pose;
	glm::mat4 transform;
	int id;
};