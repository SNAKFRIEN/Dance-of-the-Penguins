#pragma once

#include <vector>

#include "glm/glm.hpp"

#include "Penguin.h"

class IceRink;

class IceSkaterCollider
{
public:
	IceSkaterCollider(const glm::mat4& ownerTransform);
	bool IsInRink(const IceRink& rink) const;
	bool IsCollidingWithPenguin(const std::vector<Penguin>& penguins) const;
private:
	const glm::mat4& ownerTransform;
	static constexpr float collisionRadius = 0.15f;
	static constexpr float minPenguinDistance = collisionRadius + Penguin::personalSpaceRadius;
	static constexpr float minPenguinDistanceSquared = minPenguinDistance * minPenguinDistance;
};