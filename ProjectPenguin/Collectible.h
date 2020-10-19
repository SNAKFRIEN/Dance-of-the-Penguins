#pragma once

#include "glm/glm.hpp"

#include "Model.h"
#include "CircleCollider.h"

class Collectible
{
public:
	Collectible(glm::vec3 pos);
	Collectible(const Collectible& rhs);
	Collectible operator=(const Collectible& rhs);

	void Update(float dt);
	void Draw(Camera& camera);

	CircleCollider& GetCollider();
	glm::vec3 GetPos() const;
private:
	glm::vec3 pos;
	float rotation = 0.0f;

	static constexpr float hoverHeight = 0.5f;
	static constexpr float fallSpeed = 1.0f;
	static constexpr float rotationSpeed = 1.0f;
	static constexpr float pickupRange = 0.75f;

	CircleCollider collider;

	glm::mat4 transform;

	Model model;
};