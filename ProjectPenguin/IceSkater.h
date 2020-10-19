#pragma once

#include "glm/glm.hpp"

#include "AnimatedModel.h"
#include "CircleCollider.h"

class Camera;
class Input;
class Penguin;
class IceRink;
class FishingPenguin;
class PenguinStack;

class IceSkater
{
public:
	IceSkater(glm::vec3 pos);

	bool IsColliding(std::vector<Penguin>& penguins, std::unique_ptr<FishingPenguin>& fishingPenguin, std::unique_ptr<PenguinStack>& penguinStack, const IceRink& rink);
	void Update(float dt, const Input& input);
	void UpdateAnimation(float dt);
	void Draw(Camera& camera);

	void Reset();	//Sets the player back to the starting location

	glm::vec3 GetPos() const;
	glm::vec3 GetForward() const;
	CircleCollider& GetCollider();
private:
	bool IsOutOfRink(const IceRink& rink);
	bool IsCollidingWithPenguin(std::vector<Penguin>& penguins);
	bool IsCollidingWithFishingPenguin(std::unique_ptr<FishingPenguin>& fishingPenguin);
	bool IsCollidingWithPenguinStack(std::unique_ptr<PenguinStack>& penguinStack);
private:
	glm::vec3 pos;

	static constexpr float speed = 7.5f;
	static constexpr float rotationSpeed = 150.0f;

	AnimatedModel model;

	static constexpr float collisionRadius = 0.15f;
	CircleCollider collider;

	glm::mat4 rotation;
	glm::mat4 transform;
};