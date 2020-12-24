#pragma once

#include "SmokeEffect.h"

class Camera;

//This class handles spawning and destroying the smoke particle effect

class SmokeMachine
{
public:
	SmokeMachine();
	void SpawnSmoke(glm::vec3 pos);
	void Update(float deltaTime);
	void Draw(const Camera& camera);
	void Clear();
private:
	std::vector<SmokeEffect> smokeEffects;
};