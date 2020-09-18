#pragma once

#include "glm/glm.hpp"

#include "AnimatedModel.h"
#include "AnimatedJointAttachment.h"
#include "AudioSource.h"
#include "CircleCollider.h"

#include <random>

class PenguinStack
{
private:
	//PenguinNode represents any penguin that isn't the bottom one
	class PenguinNode
	{
	public:
		PenguinNode(const AnimatedModel& parentModel, int nStackedRemaining);
		void UpdateAnimation(float dt);
		void Draw(Camera& camera);
	private:
		AnimatedJointAttachment model;
		std::unique_ptr<PenguinNode> nextNode = nullptr;
	};
public:
	PenguinStack(glm::vec3 pos, glm::vec3 target, std::mt19937& rng, AudioManager& audioManager);

	void Update(float dt);
	void UpdateAnimation(float dt);
	void Draw(Camera& camera);

	//REMOVE unused function
	glm::vec3 GetPos() const;
	CircleCollider& GetCollider();
private:
	//Movement
	glm::vec3 pos;
	const glm::vec3 direction;
	static constexpr float speed = 6.0f;

	glm::mat4 transform;

	//Visuals
	AnimatedModel model;
	std::mt19937& rng;
	std::uniform_int_distribution<int> randomNStackedPenguins;
	std::unique_ptr<PenguinNode> firstNode = nullptr;

	//Collisions
	static constexpr float collisionRadius = 0.25f;
	CircleCollider collider;
};