#pragma once

#include <glm/glm.hpp>
#include "AnimatedModel.h"
#include <random>

class Camera;
class IceRink;

class Penguin
{
private:
	enum class State
	{
		Walking,
		Thinking
	};
public:
	//REPLACE: Add destructor?
	Penguin(glm::vec3 pos, bool initModel = true);
	Penguin(const Penguin& rhs);
	Penguin operator=(const Penguin& rhs) = delete;
	Penguin(Penguin&& rhs) noexcept;
	Penguin operator=(Penguin&& rhs) = delete;
	
	void Collide(int index, std::vector<Penguin>& penguins, const IceRink& rink);
	void Update(float dt);
	void Draw(Camera& camera);

	glm::vec3 GetPos() const;
	const AnimatedModel& GetModel() const;

	static constexpr float personalSpaceRadius = 0.25f;	//Makes sure penguins don't collide
	static constexpr float minPenguinDistance = personalSpaceRadius * 2.0f;	//Minimum distance between two penguins
	static constexpr float minPenguinDistanceSquared = minPenguinDistance * minPenguinDistance;
	static constexpr float minDistanceFromRinkEdges = 0.5f;
private:
	void InitModel();
	void SetState(State newState);
	void ResolveCollision(Penguin& other, float distanceSquared, glm::vec3 difference);
private:
	glm::vec3 pos;

	glm::mat4 transform;

	std::unique_ptr<AnimatedModel> model;

	//Gameplay
	glm::vec3 direction;	//Must be normalized at all times
	float speed = 0.0075f;

	State state = State::Walking;	//Always use SetState() to make sure the animation is changed too

	std::mt19937 rng;
	const std::uniform_real_distribution<float> minMaxWalkTime;
	const std::uniform_real_distribution<float> minMaxThinktime;
	float stateCountDown;
};