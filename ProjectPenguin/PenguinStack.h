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
	enum class State
	{
		Skating,
		Crashing
	};
	struct FallingPenguin
	{
		FallingPenguin() : model("Goopie.gltf", transform, "GoombaFalling") {}
		AnimatedModel model;
		glm::mat4 transform;
		float speed;
		float rotationOffset;
		float distanceMultiplier;
		float height;
		float flipSpeed;
	};
public:
	PenguinStack(glm::vec3 pos, glm::vec3 target, std::mt19937& rng, AudioManager& audioManager);

	void Update(float dt, const IceRink& rink);
	void UpdateAnimation(float dt);
	void Draw(Camera& camera);
	void Crash();

	//REMOVE unused function
	glm::vec3 GetPos() const;
	CircleCollider& GetCollider();
	bool IsCrashing();
private:
	State state = State::Skating;

	std::vector<FallingPenguin> fallingPenguins;

	//Movement
	glm::vec3 pos;
	const glm::vec3 direction;
	static constexpr float speed = 6.0f;

	float fallingTime = 0.0f;
	std::uniform_real_distribution<float> randomFallingSpeed;
	std::uniform_real_distribution<float> randomFallingRotationOffset;
	std::uniform_real_distribution<float> randomFallingDistanceMultiplier;
	std::uniform_real_distribution<float> randomFallingCurveHeight;
	std::uniform_real_distribution<float> randomFallingFlipSpeed;

	glm::mat4 transform;
	const glm::mat4 rotationMatrix;
	
	//Visuals
	int nPenguins = 0;
	AnimatedModel model;
	std::mt19937& rng;
	std::uniform_int_distribution<int> randomNStackedPenguins;
	std::unique_ptr<PenguinNode> firstNode = nullptr;

	//Collisions
	static constexpr float collisionRadius = 0.25f;
	CircleCollider collider;
};