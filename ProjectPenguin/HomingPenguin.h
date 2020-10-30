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
	HomingPenguin(const HomingPenguin& rhs);
	HomingPenguin operator=(const HomingPenguin& rhs);
	HomingPenguin(HomingPenguin&& rhs) noexcept;
	HomingPenguin operator=(HomingPenguin&& rhs) = delete;

	void Update(IceSkater& player, std::vector<Collectible>& flowers, const IceRink& rink, float dt);
	void UpdateAnimation(float dt);
	void Draw(Camera& camera);

	void GiveFlower();

	CircleCollider& GetCollider();
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

	static constexpr float wallAvoidSpeed = 2.5f;	//Speed used to avoid walls
	static constexpr float baseSpeed = 3.0f;
	static constexpr float playerHomingSpeed = 9.0f;	//Speed to pursue the player at
	float speed = baseSpeed;
	static constexpr float baseRotationSpeed = 50.0f;
	static constexpr float playerHomingRotationSpeed = 100.0f;
	float rotationSpeed = baseRotationSpeed;

	static constexpr glm::vec3 leftWallScannerBasePos = glm::vec3(-2.0f, 0.0f, -3.0f);	//These are used to make sure ice skating penguin avoids walls
	static constexpr glm::vec3 rightWallScannerBasePos = glm::vec3(2.0f, 0.0f, -3.0f);
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