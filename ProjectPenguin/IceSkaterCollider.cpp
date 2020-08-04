#include "IceSkaterCollider.h"

IceSkaterCollider::IceSkaterCollider(const glm::mat4& ownerTransform)
	:
	ownerTransform(ownerTransform)
{
}

bool IceSkaterCollider::IsInRink(const IceRink& rink)
{
	return false;
}

bool IceSkaterCollider::IsCollidingWithPenguin(const std::vector<Penguin>& penguins)
{
	return false;
}
