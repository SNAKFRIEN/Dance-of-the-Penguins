#include "JointAttachment.h"

#include "AnimatedModel.h"

JointAttachment::JointAttachment(std::string name, const AnimatedModel& parentModel, std::string joint)
	:
	model(name, transform),
	pose(parentModel.GetPose()),
	ownerModelTransform(parentModel.GetTransform())
{
	id = parentModel.GetJointIndex(joint);
}

void JointAttachment::Draw(Camera& camera)
{
	transform = ownerModelTransform * pose[id];
	model.AddToRenderQueue(camera);
}