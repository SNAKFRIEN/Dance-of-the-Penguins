#include "Penguin.h"

#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/norm.hpp>

#include "IceRink.h"

Penguin::Penguin(glm::vec3 pos, AudioManager& audioManager, bool initModel)
	:
	pos(pos),
	
	rng(std::random_device()()),
	minMaxWalkTime(1.0f, 5.0f),
	minMaxThinktime(1.0f, 3.0f),
	stateCountDown(minMaxWalkTime(rng)),
	audioManager(audioManager),
	quackSound("Quack.wav", audioManager)
{
	if (initModel)
	{
		InitModel();
	}

	glm::vec2 newDir = glm::circularRand(1.0f);
	direction = glm::vec3(newDir.x, 0.0, newDir.y);
	SetState(State::Walking);
}

Penguin::Penguin(const Penguin& rhs)
	:
	rng(std::random_device()()),
	minMaxWalkTime(1.0f, 5.0f),
	minMaxThinktime(1.0f, 3.0f),
	audioManager(rhs.audioManager),
	quackSound("Quack.wav", audioManager)
{
	InitModel();
	pos = rhs.pos;
	transform = rhs.transform;
	direction = rhs.direction;
	speed = rhs.speed;
	SetState(rhs.state);
	stateCountDown = rhs.stateCountDown;
}

Penguin::Penguin(Penguin&& rhs) noexcept
	:
	model(std::move(rhs.model)),
	rng(std::random_device()()),
	minMaxWalkTime(1.0f, 5.0f),
	minMaxThinktime(1.0f, 3.0f),
	audioManager(rhs.audioManager),
	quackSound(std::move(rhs.quackSound))
{
	pos = rhs.pos;
	transform = rhs.transform;
	direction = rhs.direction;
	speed = rhs.speed;
	SetState(rhs.state);
	stateCountDown = rhs.stateCountDown;
}

void Penguin::Collide(int index, std::vector<Penguin>& penguins, const IceRink& rink)
{
	//Collide with other penguins
	for (int i = index + 1; i < penguins.size(); i++)
	{
		glm::vec3 difference = pos - penguins[i].pos;
		float distanceSquared = glm::length2(difference);

		if (distanceSquared == 0.0f)
		{
			//Position of the two penguins is exactly the same
			//Randomly select a direction to split the two penguins
			glm::vec2 newDir = glm::circularRand(1.0f);
			glm::vec3 splitDirection = glm::vec3(newDir.x, 0.0f, newDir.y);
			pos += splitDirection * personalSpaceRadius;
			penguins[i].pos -= splitDirection * personalSpaceRadius;
		}
		else if (distanceSquared < minPenguinDistanceSquared)
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


	//Stay within the rink

	//calculation is reduced to only one corner of the rink by using absolute position
	glm::vec2 absPos(abs(pos.x), abs(pos.z));
	//Rink consists of two rectangles and 4 circles making up the corners
	glm::vec2 topRight0(rink.GetRight(), rink.GetTop() - rink.GetCornerRadius());
	glm::vec2 topRight1(rink.GetRight() - rink.GetCornerRadius(), rink.GetTop());

	//REPLACE: Room for further optimization by skipping the circle check if the penguin is within one of the rectangles
	bool inRect = false;
	if (absPos.y <= topRight0.y)
	{
		if (absPos.x + minDistanceFromRinkEdges > topRight0.x)
		{
			//Too far right, resolve for rect0
			if (pos.x < 0.0f)
			{
				pos.x = -topRight0.x + minDistanceFromRinkEdges;
			}
			else
			{
				pos.x = topRight0.x - minDistanceFromRinkEdges;
			}
			SetState(State::Thinking);
		}
		inRect = true;
	}
	else if (absPos.x <= topRight1.x)
	{
		if (absPos.y + minDistanceFromRinkEdges > topRight1.y)
		{
			//Too far up, resolve for rect1
			if (pos.z < 0.0f)
			{
				pos.z = -topRight1.y + minDistanceFromRinkEdges;
			}
			else
			{
				pos.z = topRight1.y - minDistanceFromRinkEdges;
			}
			SetState(State::Thinking);
		}
		inRect = true;
	}
	if(!inRect)
	{
		glm::vec2 difference = absPos - glm::vec2(rink.GetRight() - rink.GetCornerRadius(), rink.GetTop() - rink.GetCornerRadius());
		float bigDeal = glm::length2(difference) - ((rink.GetCornerRadius() - minDistanceFromRinkEdges) * (rink.GetCornerRadius() - minDistanceFromRinkEdges));
		if (glm::length2(difference) > ((rink.GetCornerRadius() - minDistanceFromRinkEdges) * (rink.GetCornerRadius() - minDistanceFromRinkEdges)))
		{
			//Left rink via corner, resolve for circle

			//Store signs of original pos to make sure collision is resolved for the correct corner
			glm::vec2 signMultiplier(1.0f);
			if (pos.x < 0.0f)
			{
				signMultiplier.x = -1.0f;
			}
			if (pos.z < 0.0f)
			{
				signMultiplier.y = -1.0f;
			}

			glm::vec2 resolveDirection = glm::normalize(difference);
			glm::vec2 circleCenter = glm::vec2(rink.GetRight() - rink.GetCornerRadius(), rink.GetTop() - rink.GetCornerRadius());
			absPos = circleCenter + (resolveDirection * (rink.GetCornerRadius() - minDistanceFromRinkEdges - 0.001f));

			pos = glm::vec3(absPos.x * signMultiplier.x, 0.0f, absPos.y * signMultiplier.y);

			SetState(State::Thinking);
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
			quackSound.SetPos(pos);
			quackSound.SetVel(direction * speed);
			quackSound.Play();
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
}

void Penguin::UpdateAnimation(float dt)
{
	model->Update(dt);
}

void Penguin::Draw(Camera& camera)
{
	model->AddToRenderQueue(camera);
}

glm::vec3 Penguin::GetPos() const
{
	return pos;
}

const AnimatedModel& Penguin::GetModel() const
{
	return *model;
}

void Penguin::InitModel()
{
	model = std::make_unique<AnimatedModel>("Goopie.gltf", transform);
}

void Penguin::SetState(State newState)
{
	state = newState;
	//REMOVE model is only optional thanks to unit testing. This if statement is actually redundant
	if (model)
	{
		switch (state)
		{
		case State::Thinking:
			model->SetAnimation("Idle");
			break;
		case State::Walking:
			model->SetAnimation("Waddle");
			break;
		}
	}
}

void Penguin::ResolveCollision(Penguin& other, float distanceSquared, glm::vec3 difference)
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
