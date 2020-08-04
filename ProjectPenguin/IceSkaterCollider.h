#pragma once

#include <vector>

#include "glm/glm.hpp"

class IceRink;
class Penguin;

class IceSkaterCollider
{
public:
	IceSkaterCollider(const glm::mat4& ownerTransform);
	bool IsInRink(const IceRink& rink);
	bool IsCollidingWithPenguin(const std::vector<Penguin>& penguins);
private:
	const glm::mat4& ownerTransform;
	static constexpr float collisionRadius = 0.15f;
};