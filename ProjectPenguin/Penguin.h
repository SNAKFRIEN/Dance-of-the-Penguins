#pragma once

#include "glm/glm.hpp"
#include "AnimatedModel.h"
#include <random>

class Camera;

enum class State
{
	Walking,
	Thinking
};

class Penguin
{
public:
	Penguin(glm::vec3 pos);
	Penguin(const Penguin& rhs);
	Penguin operator=(const Penguin& rhs) = delete;
	Penguin(Penguin&& rhs) noexcept;
	Penguin operator=(Penguin&& rhs) = delete;
	
	void Collide(int index, std::vector<Penguin>& penguins);	//Loop through all penguins that come after this one
	void Update(float dt);
	void Draw(Camera& camera);
private:
	void SetState(State newState);
private:
	glm::vec3 pos;

	glm::mat4 transform;

	AnimatedModel model;

	//Gameplay
	glm::vec3 direction;	//Must be normalized at all times
	float speed = 0.01f;

	State state = State::Walking;	//Always use SetState() to make sure the animation is changed too

	std::mt19937 rng;
	const std::uniform_real_distribution<float> minMaxWalkTime;
	const std::uniform_real_distribution<float> minMaxThinktime;
	float stateCountDown;

	static constexpr float personalSpaceRadius = 0.25f;	//Makes sure penguins don't collide
};