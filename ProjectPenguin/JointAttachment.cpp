#include "JointAttachment.h"

#include "AnimatedModel.h"

JointAttachment::JointAttachment(std::string name, const AnimatedModel& parentModel, std::string joint, std::string vertShader, std::string fragShader)
	:
	name(name),
	jointName(joint),
	model(name, transform, vertShader, fragShader),
	pose(parentModel.GetPose()),
	ownerModelTransform(parentModel.GetTransform())
{
	id = parentModel.GetJointIndex(joint);
}

JointAttachment::JointAttachment(const JointAttachment& rhs)
	:
	name(rhs.name),
	jointName(rhs.jointName),
	model(rhs.name, transform),
	ownerModelTransform(rhs.ownerModelTransform),
	pose(rhs.pose),
	id(rhs.id)
{
}

JointAttachment JointAttachment::operator=(const JointAttachment& rhs)
{
	assert(name == rhs.name);
	assert(jointName == rhs.jointName);
	return *this;
}

JointAttachment::JointAttachment(JointAttachment&& rhs) noexcept
	:
	name(rhs.name),
	jointName(rhs.jointName),
	model(rhs.name, transform),
	ownerModelTransform(rhs.ownerModelTransform),
	pose(rhs.pose),
	id(rhs.id)
{
}

void JointAttachment::Draw(Camera& camera)
{
	transform = ownerModelTransform * pose[id];
	model.AddToRenderQueue(camera);
}