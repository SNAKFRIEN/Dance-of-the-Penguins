#pragma once

#include <glm/glm.hpp>

#include "AnimatedModel.h"
#include "JointAttachment.h"
#include "Model.h"
#include "AudioSource.h"

class Camera;

class FishingPenguin
{
private:
	enum class State
	{
		Fishing,
		LookingAround
	};
public:
	FishingPenguin(glm::vec3 pos, float rotation, AudioManager& audioManager);
	
	void UpdateAnimation(float dt);
	void Draw(Camera& camera);
private:
	void SetState(State newState);
private:
	glm::mat4 transform;

	AnimatedModel model;
	JointAttachment fishingRod;
	JointAttachment bucket;
	Model crate;

	State state = State::Fishing;	//Always use SetState() to make sure the animation is changed too

	AudioManager& audioManager;
};