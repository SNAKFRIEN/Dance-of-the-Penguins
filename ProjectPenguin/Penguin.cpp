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
	if (state == State::Walking)
	{
		for (int i = 0; i < penguins.size(); i++)
		{
			if (i != index)
			{
				glm::vec3 difference = pos - penguins[i].pos;
				float distance = glm::length(difference);
				
				if (distance < personalSpaceRadius * 2.0f)
				{
					//If two penguins are in exactly the same position, separate them in a random direction
					if (pos == penguins[i].pos)
					{
						//Randomly select a direction split the two penguins
						glm::vec2 newDir = glm::circularRand(1.0f);
						glm::vec3 splitDirection = glm::vec3(newDir.x, 0.0f, newDir.y);
						pos += splitDirection * (personalSpaceRadius * 0.5f);
						penguins[i].pos -= splitDirection * (personalSpaceRadius * 0.5f);
					}
					else
					{
						glm::vec3 collisionNormal = glm::normalize(difference);
						//Point the penguins away from each other
						//direction = collisionNormal;
						//penguins[i].direction = -collisionNormal;
						//Put some space between those penguins
						pos += collisionNormal * (personalSpaceRadius * 2.0f - distance + 0.001f);
						//Set state to thinking
						SetState(State::Thinking);
					}
				}
			}
		}
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
