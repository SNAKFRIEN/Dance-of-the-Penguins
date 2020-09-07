#include "CircleCollider.h"

#include <glm/gtx/norm.hpp>
#include <glm/gtc/random.hpp>

#include "IceRink.h"

CircleCollider::CircleCollider(glm::vec3& ownerPos, const float& radius)
	:
	ownerPos(ownerPos),
	radius(radius)
{
}

CollisionData CircleCollider::CalculateCollision(CircleCollider& other)
{
	CollisionData result;
	result.difference = ownerPos - other.ownerPos;
	result.distanceSquared = glm::length2(result.difference);

	const float minDistanceSquared = pow(radius + other.radius, 2.0f);

	if (result.distanceSquared == 0.0f)
	{
		//Position of the two penguins is exactly the same
		result.isColliding = true;
		result.isSamePos = true;
		result.colliderA = this;
		result.colliderB = &other;
	}
	else if (result.distanceSquared < minDistanceSquared)
	{
		result.isColliding = true;
		result.isSamePos = false;
		result.colliderA = this;
		result.colliderB = &other;
	}
	return result;
}

void CircleCollider::ResolveSamePos(CollisionData& collisionData)
{
	assert(collisionData.isColliding);
	assert(collisionData.isSamePos);

	//Randomly select a direction to split the two colliders
	glm::vec2 newDir = glm::circularRand(1.0f);
	glm::vec3 splitDirection = glm::vec3(newDir.x, 0.0f, newDir.y);
	collisionData.colliderA->ownerPos += splitDirection * radius;
	collisionData.colliderB->ownerPos -= splitDirection * radius;
}

void CircleCollider::ResolveDifferentPos(CollisionData& collisionData)
{
	assert(collisionData.colliderA == this);
	assert(collisionData.isColliding);

	//Calculate distance between colliders
	float distance = sqrt(collisionData.distanceSquared);
	//Calculate the collision normal by normalizing the difference
	glm::vec3 collisionNormal = collisionData.difference / distance;
	//Put some space between the colliders
	ownerPos += collisionNormal * ((radius + collisionData.colliderB->radius) - distance + 0.001f);
}

bool CircleCollider::IsInRink(const IceRink& rink) const
{
	//calculation is reduced to only one corner of the rink by using absolute position
	glm::vec2 absPos(abs(ownerPos.x), abs(ownerPos.z));
	//Rink consists of two rectangles and 4 circles making up the corners
	glm::vec2 topRight0(rink.GetRight(), rink.GetTop() - rink.GetCornerRadius());
	glm::vec2 topRight1(rink.GetRight() - rink.GetCornerRadius(), rink.GetTop());

	return (absPos.x + radius <= topRight0.x && absPos.y + radius < topRight0.y)
		|| (absPos.x + radius <= topRight1.x && absPos.y + radius < topRight1.y)
		|| glm::length(absPos - glm::vec2(rink.GetRight() - rink.GetCornerRadius(), rink.GetTop() - rink.GetCornerRadius()))
		<= (rink.GetCornerRadius() - radius);
}
