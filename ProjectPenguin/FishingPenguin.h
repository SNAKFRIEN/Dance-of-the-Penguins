#pragma once

#include <glm/glm.hpp>

#include "AnimatedModel.h"
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

	std::unique_ptr<AnimatedModel> model;

	State state = State::Fishing;	//Always use SetState() to make sure the animation is changed too

	AudioManager& audioManager;
};