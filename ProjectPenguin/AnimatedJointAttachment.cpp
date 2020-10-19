#include "AnimatedJointAttachment.h"

AnimatedJointAttachment::AnimatedJointAttachment(std::string name, const AnimatedModel& parentModel, std::string joint, std::string animationName, std::string vertShader, std::string fragShader)
	:
	name(name),
	jointName(joint),
	model(name, transform, animationName, vertShader, fragShader),
	pose(parentModel.GetPose()),
	ownerModelTransform(parentModel.GetTransform())
{
	id = parentModel.GetJointIndex(joint);
}

AnimatedJointAttachment::AnimatedJointAttachment(const AnimatedJointAttachment& rhs)
	:
	name(rhs.name),
	jointName(rhs.jointName),
	model(rhs.name, transform, rhs.GetModel().GetAnimation()),
	ownerModelTransform(rhs.ownerModelTransform),
	pose(rhs.pose),
	id(rhs.id)
{
}

AnimatedJointAttachment AnimatedJointAttachment::operator=(const AnimatedJointAttachment& rhs)
{
	assert(name == rhs.name);
	assert(jointName == rhs.jointName);
	return *this;
}

AnimatedJointAttachment::AnimatedJointAttachment(AnimatedJointAttachment&& rhs) noexcept
	:
	name(rhs.name),
	jointName(rhs.jointName),
	model(rhs.name, rhs.transform, rhs.GetModel().GetAnimation()),
	ownerModelTransform(rhs.ownerModelTransform),
	pose(rhs.pose),
	id(rhs.id)
{
}

void AnimatedJointAttachment::UpdateAnimation(float dt)
{
	model.Update(dt);
}

void AnimatedJointAttachment::Draw(Camera& camera)
{
	transform = ownerModelTransform * pose[id];
	model.AddToRenderQueue(camera);
}

void AnimatedJointAttachment::SetAnimation(std::string name)
{
	model.SetAnimation(name);
}

const AnimatedModel& AnimatedJointAttachment::GetModel() const
{
	return model;
}
