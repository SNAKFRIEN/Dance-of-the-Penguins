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
		Crashing
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
	HomingPenguin operator=(HomingPenguin&& rhs) noexcept;

	void Update(IceSkater& player, std::vector<Collectible>& flowers, const IceRink& rink, float dt);
	void UpdateAnimation(float dt);
	void Draw(Camera& camera);

	void GiveFlower();
	void Collide(const IceRink& rink);	//REPLACE: add collision with fishingpenguin and normal penguins?

	CircleCollider& GetCollider();
	glm::vec3 GetForward() const;
	glm::vec3 GetPos() const;
	bool IsLockedOn() const;
	bool IsFinished() const;
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
	static constexpr float playerHomingSpeed = 8.0f;	//Speed to pursue the player at
	static constexpr float playerHomingAcceleration = 5.0f;
	float speed = baseSpeed;
	static constexpr float baseRotationSpeed = 50.0f;
	static constexpr float playerHomingRotationSpeed = 75.0f;
	float rotationSpeed = baseRotationSpeed;

	float fallingTime = 0.0f;
	static constexpr float fallingSpeed = 1.0f;
	static constexpr glm::vec3 fallingCurveScale = glm::vec3(0.0f, 4.0f, 6.0f);
	static constexpr float fallingFlipSpeed = glm::radians(250.0f);

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
	std::unique_ptr<JointAttachment> flower = nullptr;

	static constexpr float collisionRadius = 0.25f;
	CircleCollider collider;

	bool finished = false;	//Set to true once this homingPenguin can be erased from the vector
};