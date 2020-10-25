#include "Collectible.h"

#include "glm/gtc/matrix_transform.hpp"

Collectible::Collectible(glm::vec3 inPos)
	:
	pos(inPos),
	model("Bouquet.gltf", transform),
	collider(pos, pickupRange)
{
	//REMOVE
	std::cout << "Constructed new Bouquet at " << pos.x << ", " << pos.z << std::endl;
}

Collectible::Collectible(const Collectible& rhs)
	:
	pos(rhs.pos),
	model("Bouquet.gltf", transform),
	collider(pos, pickupRange)
{
	pos = rhs.pos;
	rotation = rhs.rotation;
	//REMOVE
	std::cout << "Constructed a copy of a Bouquet at " << pos.x << ", " << pos.z << std::endl;
}

Collectible Collectible::operator=(const Collectible& rhs)
{
	//REMOVE
	std::cout << "Copied a bouquet at " << rhs.pos.x << ", " << rhs.pos.z << " to " << pos.x << ", " << pos.z << std::endl;

	pos = rhs.pos;
	rotation = rhs.rotation;
	return *this;
}

Collectible::Collectible(Collectible&& rhs) noexcept
	:
	pos(rhs.pos),
	model("Bouquet.gltf", transform),
	collider(pos, pickupRange)
{
	pos = rhs.pos;
	rotation = rhs.rotation;
	//REMOVE
	std::cout << "Constructed a copy of a Bouquet at " << pos.x << ", " << pos.z << std::endl;
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
