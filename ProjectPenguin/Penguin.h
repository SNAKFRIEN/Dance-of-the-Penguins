#pragma once

#include "glm/glm.hpp"
#include "Model.h"
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
	
	void Update(float dt);
	void Draw(Camera& camera);
private:
	void ChangeDirection();

private:
	glm::vec3 pos;

	glm::mat4 transform;

	Model model;

	//Gameplay
	glm::vec3 direction;	//Must be normalized at all times
	float speed = 0.01f;

	State state = State::Walking;

	std::mt19937 rng;
	const std::uniform_real_distribution<float> minMaxWalkTime;
	const std::uniform_real_distribution<float> minMaxThinktime;
	float stateCountDown;
};