#pragma once

#include "Plus5Effect.h"

class Camera;

//This class handles spawning and destroying the smoke particle effect

class Plus5EffectDispenser
{
public:
	Plus5EffectDispenser();
	void Dispense(glm::vec3 pos);
	void Update(float deltaTime);
	void Draw(const Camera& camera);
	void Clear();
private:
	std::vector<Plus5Effect> plus5Effects;
};