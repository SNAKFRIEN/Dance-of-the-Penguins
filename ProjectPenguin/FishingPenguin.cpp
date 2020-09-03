#include "FishingPenguin.h"

FishingPenguin::FishingPenguin(glm::vec3 pos, float rotation, AudioManager& audioManager)
	:
	transform(1.0f),
	audioManager(audioManager),
	model(std::make_unique<AnimatedModel>("Goopie.gltf", transform))
{
	SetState(State::Fishing);

	//transform = glm::rotate(transform, rotation, glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::translate(transform, pos);
}

void FishingPenguin::UpdateAnimation(float dt)
{
	model->Update(dt);
}

void FishingPenguin::Draw(Camera& camera)
{
	model->AddToRenderQueue(camera);
}

void FishingPenguin::SetState(State newState)
{
	state = newState;
	switch (state)
	{
	case State::Fishing:
		model->SetAnimation("FishingIdle");
		break;
	case State::LookingAround:
		model->SetAnimation("FishingLooking");
		break;
	}
}
