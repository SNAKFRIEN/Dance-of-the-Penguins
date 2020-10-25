#pragma once

#include <glm/glm.hpp>

#include "AnimatedModel.h"
#include "AudioSource.h"
#include "CircleCollider.h"
#include "JointAttachment.h"

#include <random>

class Camera;
class IceSkater;
class Collectible;
class IceRink;

class HomingPenguin
{
private:
	enum class State
	{
		Roaming,
		HomingFlower,
		HomingPlayer,
		Falling
	};
	enum class RoamingDirection
	{
		Left,
		Right
	};
public:
	HomingPenguin(glm::vec3 pos);

	void Update(IceSkater& player, std::vector<Collectible>& flowers, const IceRink& rink, float dt);
	void UpdateAnimation(float dt);
	void Draw(Camera& camera);

	glm::vec3 GetForward() const;
private:
	std::mt19937 rng;
	const std::uniform_real_distribution<float> randomSwerveTime;
	const std::uniform_real_distribution<float> randomRotation;
	float swerveTimer;

	State state = State::Roaming;
	RoamingDirection roamingDirection = RoamingDirection::Left;

	glm::vec3 pos;
	float rotation;
	glm::mat4 transform;

	float speed = 3.0f;
	static constexpr float rotationSpeed = 50.0f;

	static constexpr glm::vec3 leftWallScannerBasePos = glm::vec3(-2.0f, 0.0f, -2.0f);	//These are used to make sure ice skating penguin avoids walls
	static constexpr glm::vec3 rightWallScannerBasePos = glm::vec3(2.0f, 0.0f, -2.0f);
	glm::vec3 leftWallScannerPos;
	glm::vec3 rightWallScannerPos;
	static constexpr float wallScannerRadii = 0.05f;
	CircleCollider leftWallScanner;
	CircleCollider rightWallScanner;


	static constexpr float scanRadius = 5.0f;
	CircleCollider flowerScanner;

	AnimatedModel model;

	static constexpr float collisionRadius = 0.25f;
	CircleCollider collider;
};