#include "Penguin.h"

#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

Penguin::Penguin(glm::vec3 pos)
	:
	pos(pos),
	model("Goppie.gltf", transform),
	rng(std::random_device()()),
	minMaxWalkTime(1.0f, 5.0f),
	minMaxThinktime(1.0f, 3.0f),
	stateCountDown(minMaxWalkTime(rng))
{
	glm::vec2 newDir = glm::circularRand(1.0f);
	direction = glm::vec3(newDir.x, 0.0, newDir.y);
}

Penguin::Penguin(const Penguin& rhs)
	:
	model("Goppie.gltf", transform),
	rng(std::random_device()()),
	minMaxWalkTime(1.0f, 5.0f),
	minMaxThinktime(1.0f, 3.0f)
{
	pos = rhs.pos;
	transform = rhs.transform;
	direction = rhs.direction;
	speed = rhs.speed;
	state = rhs.state;
	stateCountDown = rhs.stateCountDown;
}

Penguin::Penguin(Penguin&& rhs)
	:
	model("Goppie.gltf", transform),
	rng(std::random_device()()),
	minMaxWalkTime(1.0f, 5.0f),
	minMaxThinktime(1.0f, 3.0f)
{
	pos = rhs.pos;
	transform = rhs.transform;
	direction = rhs.direction;
	speed = rhs.speed;
	state = rhs.state;
	stateCountDown = rhs.stateCountDown;
}

void Penguin::Update(float dt)
{
	stateCountDown -= dt;
	switch (state)
	{
	case State::Walking:
		model.SetAnimation("waddle_neutral_fast");
		pos += direction * speed;
		if (stateCountDown < 0.0f)
		{
			state = State::Thinking;
			
			stateCountDown = minMaxThinktime(rng);
		}
		break;
	case State::Thinking:
		model.SetAnimation("Thunking");
		if (stateCountDown < 0.0f)
		{
			state = State::Walking;
			glm::vec2 newDir = glm::circularRand(1.0f);
			direction = glm::vec3(newDir.x, 0.0, newDir.y);
			stateCountDown = minMaxWalkTime(rng);
		}
		break;
	}

	transform = glm::translate(glm::mat4(1.0f), pos) * glm::orientation(direction, glm::vec3(0.0f, 0.0f, -1.0f));

	//Update animation
	model.Update(0.01f);
}

void Penguin::Draw(Camera& camera)
{
	model.AddToRenderQueue(camera);
}

void Penguin::ChangeDirection()
{
	direction = glm::vec3(glm::circularRand(1.0f), 1.0f);
}