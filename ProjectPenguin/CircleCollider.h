#pragma once

#include "glm/glm.hpp"

class CircleCollider;
class IceRink;

struct CollisionData
{
	bool isColliding = false;	//Are the colliders colliding?
	CircleCollider* colliderA = nullptr;	//First collider
	CircleCollider* colliderB = nullptr;	//Second collider
	glm::vec3 difference;	//Vector from collider A to B
	float distanceSquared;		//Distance between A and B
	bool isSamePos;				//Are the colliders in the same position?
};

class CircleCollider
{
public:
	CircleCollider(glm::vec3& ownerPos, const float& radius);
	
	CollisionData CalculateCollision(CircleCollider& other);
	void ResolveSamePos(CollisionData& collisionData);
	void ResolveDifferentPos(CollisionData& collisionData);
	bool IsInRink(const IceRink& rink) const;

	glm::vec3 GetPos() const;
private:
	glm::vec3& ownerPos;
	const float& radius;
};