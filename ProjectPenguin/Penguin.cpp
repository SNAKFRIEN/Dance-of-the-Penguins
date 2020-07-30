#include "Penguin.h"

#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/norm.hpp>

Penguin::Penguin(glm::vec3 pos)
	:
	pos(pos),
	model("Goopie.gltf", transform),
	rng(std::random_device()()),
	minMaxWalkTime(1.0f, 5.0f),
	minMaxThinktime(1.0f, 3.0f),
	stateCountDown(minMaxWalkTime(rng))
{
	glm::vec2 newDir = glm::circularRand(1.0f);
	direction = glm::vec3(newDir.x, 0.0, newDir.y);
	SetState(State::Walking);
}

Penguin::Penguin(const Penguin& rhs)
	:
	model("Goopie.gltf", transform),
	rng(std::random_device()()),
	minMaxWalkTime(1.0f, 5.0f),
	minMaxThinktime(1.0f, 3.0f)
{
	pos = rhs.pos;
	transform = rhs.transform;
	direction = rhs.direction;
	speed = rhs.speed;
	SetState(rhs.state);
	stateCountDown = rhs.stateCountDown;
}

Penguin::Penguin(Penguin&& rhs) noexcept
	:
	model("Goopie.gltf", transform),
	rng(std::random_device()()),
	minMaxWalkTime(1.0f, 5.0f),
	minMaxThinktime(1.0f, 3.0f)
{
	pos = rhs.pos;
	transform = rhs.transform;
	direction = rhs.direction;
	speed = rhs.speed;
	SetState(rhs.state);
	stateCountDown = rhs.stateCountDown;
}

void Penguin::Collide(int index, std::vector<Penguin>& penguins)
{
	//Collide with other penguins
	for (int i = index + 1; i < penguins.size(); i++)
	{
		glm::vec3 difference = pos - penguins[i].pos;
		float distanceSquared = glm::length2(difference);
		
		if (distanceSquared < minPenguinDistanceSquared)
		{
			if (state == State::Walking)
			{
				ResolveCollision(penguins[i], distanceSquared, difference);
			}
			if (penguins[i].state == State::Walking)
			{
				penguins[i].ResolveCollision(*this, distanceSquared, -difference);
			}
		}
	}

	//Collide with boundaries
	//REPLACE: boundaries shouldn't be hardcoded
	if (pos.x + personalSpaceRadius >= 26.0f)
	{
		pos.x = 26.0f - personalSpaceRadius - 0.001f;
		SetState(State::Thinking);
	}
	else if (pos.x - personalSpaceRadius <= -26.0f)
	{
		pos.x = -26.0f + personalSpaceRadius + 0.001f;
		SetState(State::Thinking);
	}
	if (pos.z + personalSpaceRadius >= 15.0f)
	{
		pos.z = 15.0f - personalSpaceRadius - 0.001f;
		SetState(State::Thinking);
	}
	else if (pos.z - personalSpaceRadius <= -15.0f)
	{
		pos.z = -15.0f + personalSpaceRadius + 0.001f;
		SetState(State::Thinking);
	}
}

void Penguin::Update(float dt)
{
	stateCountDown -= dt;
	switch (state)
	{
	case State::Walking:
		pos += direction * speed;
		if (stateCountDown < 0.0f)
		{
			SetState(State::Thinking);
			stateCountDown = minMaxThinktime(rng);
		}
		break;
	case State::Thinking:
		if (stateCountDown < 0.0f)
		{
			SetState(State::Walking);
			glm::vec2 newDir = glm::circularRand(1.0f);
			direction = glm::vec3(newDir.x, 0.0, newDir.y);
			stateCountDown = minMaxWalkTime(rng);
		}
		break;
	}

	transform = glm::translate(glm::mat4(1.0f), pos) * glm::orientation(direction, glm::vec3(0.0f, 0.0f, -1.0f));

	//Update animation
	model.Update(dt);
}

void Penguin::Draw(Camera& camera)
{
	model.AddToRenderQueue(camera);
}

void Penguin::SetState(State newState)
{
	state = newState;
	switch (state)
	{
	case State::Thinking:
		model.SetAnimation("Idle");
		break;
	case State::Walking:
		model.SetAnimation("Waddle");
		break;
	}
}

void Penguin::ResolveCollision(Penguin& other, float distanceSquared, glm::vec3 difference)
{
	//If two penguins are in exactly the same position, separate them in a random direction
	if (pos == other.pos)
	{
		//Randomly select a direction split the two penguins
		glm::vec2 newDir = glm::circularRand(1.0f);
		glm::vec3 splitDirection = glm::vec3(newDir.x, 0.0f, newDir.y);
		pos += splitDirection * (personalSpaceRadius * 0.5f);
		other.pos -= splitDirection * (personalSpaceRadius * 0.5f);
	}
	else
	{
		//Calculate distance between the penguins
		float distance = sqrt(distanceSquared);
		//Calculate the collision normal by normalizing the difference
		glm::vec3 collisionNormal = difference / distance;
		//Put some space between those penguins
		pos += collisionNormal * (minPenguinDistance - distance + 0.001f);
		//Set state to thinking
		SetState(State::Thinking);
	}
}
