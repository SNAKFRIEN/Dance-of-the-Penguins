#include "FishingPenguin.h"

FishingPenguin::FishingPenguin(glm::vec3 pos, float rotation, AudioManager& audioManager)
	:
	transform(1.0f),
	audioManager(audioManager),
	model("Goopie.gltf", transform),
	fishingRod("FishingPole.gltf", model, "lower_arm.R"),
	bucket("Bucket.gltf", model, "head"),
	crate("Crate.gltf", transform)
{
	SetState(State::Fishing);

	transform = glm::translate(transform, pos);
	transform = glm::rotate(transform, rotation, glm::vec3(0.0f, 1.0f, 0.0f));
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
