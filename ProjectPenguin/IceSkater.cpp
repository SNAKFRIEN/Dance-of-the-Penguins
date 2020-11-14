#include "IceSkater.h"

#include "Input.h"
#include "Penguin.h"
#include "FishingPenguin.h"
#include "PenguinStack.h"
#include "HomingPenguin.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"

//REMOVE, only used for debug brake (hold space to stand still)
#include "GLFW/glfw3.h"

IceSkater::IceSkater(glm::vec3 inPos)
	:
	pos(inPos),
	model("IceSkater.gltf", transform, "Skating"),
	collider(pos, collisionRadius)
{
	rotation = glm::mat4(1.0f);
}

bool IceSkater::IsColliding(std::vector<Penguin>& penguins, std::unique_ptr<FishingPenguin>& fishingPenguin, std::unique_ptr<PenguinStack>& penguinStack, std::vector<HomingPenguin>& homingPenguins, const IceRink& rink)
{
	return IsOutOfRink(rink) || IsCollidingWithPenguin(penguins) || IsCollidingWithFishingPenguin(fishingPenguin) || IsCollidingWithPenguinStack(penguinStack) || IsCollidingWithHomingPenguin(homingPenguins);
}

void IceSkater::Update(float dt, const Input& input)
{
	bool pressed = false;
	glm::vec3 direction(0.0f);
	direction.x = input.GetRightAxis();
	direction.z = -1.0f * input.GetForwardAxis();	//Flip z to align with opengl coordinate system
	if (glm::length2(direction) >= pow(0.3f, 2.0f))
	{
		glm::normalize(direction);
		pressed = true;
	}

	float deltaRotation = 0.0f;
	if (pressed)
	{
		direction = glm::normalize(direction);
		float angle = glm::orientedAngle(direction, GetForward(), glm::vec3(0.0f, -1.0f, 0.0f));
		if (angle > 0.0f)
		{
			deltaRotation = std::min(angle, glm::radians(rotationSpeed * dt));
		}
		else if (angle < 0.0f)
		{
			deltaRotation = std::max(angle, glm::radians(-rotationSpeed * dt));
		}
		rotation = glm::rotate(rotation, deltaRotation, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	if (!input.IsPressed(GLFW_KEY_SPACE))
	{
		pos += GetForward() * speed * dt;
	}

	transform = glm::translate(glm::mat4(1.0f), pos) * rotation;
}

void IceSkater::UpdateAnimation(float dt)
{
	model.Update(dt);
}

void IceSkater::Draw(Camera& camera)
{
	model.AddToRenderQueue(camera);
}

void IceSkater::Reset()
{
	pos = glm::vec3(0.0f);
	rotation = glm::mat4(1.0f);
}

glm::vec3 IceSkater::GetPos() const
{
	return pos;
}

glm::vec3 IceSkater::GetForward() const
{
	return rotation[2];
}

CircleCollider& IceSkater::GetCollider()
{
	return collider;
}

bool IceSkater::IsOutOfRink(const IceRink& rink)
{
	return !collider.IsInRink(rink);
}

bool IceSkater::IsCollidingWithPenguin(std::vector<Penguin>& penguins)
{
	for (Penguin& p : penguins)
	{
		auto collision = collider.CalculateCollision(p.GetCollider());
		if (collision.isColliding)
		{
			return true;
		}
	}
	return false;
}

bool IceSkater::IsCollidingWithFishingPenguin(std::unique_ptr<FishingPenguin>& fishingPenguin)
{
	//Check that the fishing penguin exists
	if (!fishingPenguin)
	{
		return false;
	}

	//Check collisions with pond
	auto pondCollision = collider.CalculateCollision(fishingPenguin->GetPondCollider());
	if (pondCollision.isColliding)
	{
		return true;
	}
	
	//Check collisions with penguin
	auto penguinCollision = collider.CalculateCollision(fishingPenguin->GetPenguinCollider());
	if (penguinCollision.isColliding)
	{
		return true;
	}

	//If no collisions found, return false
	return false;
}

bool IceSkater::IsCollidingWithPenguinStack(std::unique_ptr<PenguinStack>& penguinStack)
{
	if (!penguinStack)
	{
		return false;
	}

	auto collision = collider.CalculateCollision(penguinStack->GetCollider());
	return collision.isColliding;
}

bool IceSkater::IsCollidingWithHomingPenguin(std::vector<HomingPenguin>& homingPenguins)
{
	for (auto& hp : homingPenguins)
	{
		if (collider.CalculateCollision(hp.GetCollider()).isColliding)
		{
			return true;
		}
	}
	return false;
}
