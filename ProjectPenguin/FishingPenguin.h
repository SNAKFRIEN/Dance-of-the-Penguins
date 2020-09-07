#pragma once

#include <glm/glm.hpp>

#include "AnimatedModel.h"
#include "JointAttachment.h"
#include "Model.h"
#include "AudioSource.h"
#include "CircleCollider.h"

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

	CircleCollider& GetPenguinCollider();
	CircleCollider& GetPondCollider();
private:
	void SetState(State newState);
private:
	glm::mat4 transform;

	//Visuals
	AnimatedModel model;
	JointAttachment fishingRod;
	JointAttachment bucket;
	Model crate;

	//State
	State state = State::Fishing;	//Always use SetState() to make sure the animation is changed too

	//Audio
	AudioManager& audioManager;

	//Collisions
	glm::vec3 pondPos;
	glm::vec3 penguinPos;

	static constexpr float pondRadius = 0.6f;
	static constexpr float penguinRadius = 0.25f;

	CircleCollider penguinCollider;
	CircleCollider pondCollider;
};