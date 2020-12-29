#include "FishingPenguin.h"

FishingPenguin::FishingPenguin(glm::vec3 inPos, float rotation, AudioManager& audioManager)
	:
	transform(1.0f),
	audioManager(audioManager),
	model("Goopie.gltf", transform, "FishingIdle"),
	fishingRod("FishingPole.gltf", model, "lower_arm.R"),
	bucket("Bucket.gltf", model, "head"),
	crate("Crate.gltf", transform),
	pondCollider(pondPos, pondRadius),
	penguinCollider(penguinPos, penguinRadius)
{
	SetState(State::Fishing);

	transform = glm::translate(transform, inPos);
	transform = glm::rotate(transform, rotation, glm::vec3(0.0f, 1.0f, 0.0f));
	pondPos = inPos;
	penguinPos = glm::vec3(0.0f, 0.0f, 0.95);
	penguinPos = transform * glm::vec4(penguinPos, 1.0f);
}

void FishingPenguin::UpdateAnimation(float dt)
{
	model.Update(dt);
}

void FishingPenguin::Draw(Camera& camera)
{
	model.AddToRenderQueue(camera);
	fishingRod.Draw(camera);
	bucket.Draw(camera);
	crate.AddToRenderQueue(camera);
}

CircleCollider& FishingPenguin::GetPenguinCollider()
{
	return penguinCollider;
}

CircleCollider& FishingPenguin::GetPondCollider()
{
	return pondCollider;
}

void FishingPenguin::SetPos(glm::vec3 inPos, float rotation)
{
	transform = glm::translate(glm::mat4(1.0f) , inPos);
	transform = glm::rotate(transform, rotation, glm::vec3(0.0f, 1.0f, 0.0f));
	pondPos = inPos;
	penguinPos = glm::vec3(0.0f, 0.0f, 0.95);
	penguinPos = transform * glm::vec4(penguinPos, 1.0f);
}

void FishingPenguin::SetState(State newState)
{
	state = newState;
	switch (state)
	{
	case State::Fishing:
		model.SetAnimation("FishingIdle");
		break;
	case State::LookingAround:
		model.SetAnimation("FishingLooking");
		break;
	}
}
