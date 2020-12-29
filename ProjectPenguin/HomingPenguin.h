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
class SmokeMachine;
class AudioSource;

class HomingPenguin
{
private:
	enum class State
	{
		Roaming,
		HomingCandyCane,
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

	void Update(IceSkater& player, std::vector<Collectible>& candyCanes, const IceRink& rink, float dt);
	void UpdateAnimation(float dt);
	void Draw(Camera& camera);

	void GiveCandyCane();
	void Collide(const IceRink& rink, SmokeMachine& smokeMachine, AudioSource& bonk);	//REPLACE: add collision with fishingpenguin and normal penguins?

	CircleCollider& GetCollider();
	glm::vec3 GetForward() const;
	glm::vec3 GetPos() const;
	bool IsLockedOnToPlayer() const;
	bool IsLockedOnToCollectible() const;
	bool IsFinished() const;
	bool IsCrashing() const;
	bool TargetInSight() const;	//Returns true when targeted collectible is in sight
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
	CircleCollider candyCaneScanner;
	glm::vec3 closestCandyCanePos;
	float warningFov = glm::radians(75.0f);	//Exclusively used to display yellow penguin warnings when close to a collectible

	AnimatedModel model;
	std::unique_ptr<JointAttachment> candyCane = nullptr;
	JointAttachment hat;
	JointAttachment vest;

	static constexpr float collisionRadius = 0.25f;
	CircleCollider collider;

	bool finished = false;	//Set to true once this homingPenguin can be erased from the vector
};