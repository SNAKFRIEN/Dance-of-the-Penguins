#include "Collectible.h"

#include "glm/gtc/matrix_transform.hpp"

Collectible::Collectible(glm::vec3 pos)
	:
	pos(pos),
	model("Bouquet.gltf", transform),
	collider(pos, pickupRange)
{
}

Collectible::Collectible(const Collectible& rhs)
	:
	pos(rhs.pos),
	model("Bouquet.gltf", transform),
	collider(pos, pickupRange)
{
	pos = rhs.pos;
	rotation = rhs.rotation;
}

Collectible Collectible::operator=(const Collectible& rhs)
{
	pos = rhs.pos;
	rotation = rhs.rotation;
	return *this;
}

void Collectible::Update(float dt)
{
	if (pos.y > hoverHeight)
	{
		pos.y -= fallSpeed * dt;
	}
	else
	{
		pos.y = hoverHeight;
	}
	rotation += rotationSpeed * dt;
	transform = glm::translate(glm::mat4(1.0f), pos) * glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Collectible::Draw(Camera& camera)
{
	model.AddToRenderQueue(camera);
}

CircleCollider& Collectible::GetCollider()
{
	return collider;
}

glm::vec3 Collectible::GetPos() const
{
	return pos;
}
