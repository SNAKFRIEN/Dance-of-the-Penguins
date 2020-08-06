#include "IceSkaterCollider.h"

#include <glm/gtx/norm.hpp>

#include "IceRink.h"

IceSkaterCollider::IceSkaterCollider(const glm::mat4& ownerTransform)
	:
	ownerTransform(ownerTransform)
{
}

bool IceSkaterCollider::IsInRink(const IceRink& rink) const
{
	glm::vec3 v3Pos(ownerTransform[3]);
	glm::vec2 absPos(abs(v3Pos.x), abs(v3Pos.z));
	//Rink consists of two rectangles and 4 circles making up the corners
	//calculation is reduced to only one corner by using absolute position
	glm::vec2 topRight0(rink.GetRight(), rink.GetTop() - rink.GetCornerRadius());
	glm::vec2 topRight1(rink.GetRight() - rink.GetCornerRadius(), rink.GetTop());

	return (absPos.x + collisionRadius <= topRight0.x && absPos.y + collisionRadius < topRight0.y)
		|| (absPos.x + collisionRadius <= topRight1.x && absPos.y + collisionRadius < topRight1.y)
		|| glm::length(absPos - glm::vec2(rink.GetRight() - rink.GetCornerRadius(), rink.GetTop() - rink.GetCornerRadius()))
		<= (rink.GetCornerRadius() - collisionRadius);
}

bool IceSkaterCollider::IsCollidingWithPenguin(const std::vector<Penguin>& penguins) const
{
	glm::vec3 pos(ownerTransform[3]);
	for (const Penguin& p : penguins)
	{
		if (glm::length2(p.GetPos() - pos) < minPenguinDistanceSquared)
		{
			return true;
		}
	}
	return false;
}
