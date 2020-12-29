#include "Penguin.h"

#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/norm.hpp>

#include "IceRink.h"
#include "FishingPenguin.h"

Penguin::Penguin(glm::vec3 inPos, bool initModel)
	:
	pos(inPos),
	rng(std::random_device()()),
	minMaxWalkTime(1.0f, 5.0f),
	minMaxThinktime(1.0f, 3.0f),
	stateCountDown(minMaxWalkTime(rng)),
	collider(pos, personalSpaceRadius)
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
	collider(pos, personalSpaceRadius)
{
	InitModel();
	accessories = rhs.accessories;
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
	accessories(std::move(accessories)),
	rng(std::random_device()()),
	minMaxWalkTime(1.0f, 5.0f),
	minMaxThinktime(1.0f, 3.0f),
	collider(pos, personalSpaceRadius)
{
	pos = rhs.pos;
	transform = rhs.transform;
	direction = rhs.direction;
	speed = rhs.speed;
	SetState(rhs.state);
	stateCountDown = rhs.stateCountDown;
}

void Penguin::AddAccessory(std::string name, std::string joint, std::string vertShader, std::string fragShader)
{
	accessories.emplace_back(name, *model, joint, vertShader, fragShader);
}

void Penguin::Collide(int index, std::vector<Penguin>& penguins, std::unique_ptr<FishingPenguin>& fishingPenguin, const IceRink& rink)
{
	//Collide with other penguins
	for (int i = index + 1; i < penguins.size(); i++)
	{
		auto collision = collider.CalculateCollision(penguins[i].collider);
		if (collision.isColliding)
		{
			if (collision.isSamePos)
			{
				collider.ResolveSamePos(collision);
			}
			else
			{
				if (state == State::Walking)
				{
					SetState(State::Thinking);
					collider.ResolveDifferentPos(collision);
				}
				if (penguins[i].state == State::Walking)
				{
					penguins[i].SetState(State::Thinking);
					//Reverse collision data
					auto collisionReversed = collision;
					std::swap(collisionReversed.colliderA, collisionReversed.colliderB);
					collisionReversed.difference *= -1.0f;
					penguins[i].collider.ResolveDifferentPos(collisionReversed);
				}
			}
		}
	}

	//Collide with fishingPenguin's pond
	if (fishingPenguin)
	{
		{
			auto collision = collider.CalculateCollision(fishingPenguin->GetPondCollider());
			if (collision.isColliding)
			{
				if (collision.isSamePos)
				{
					//REMOVE by improving same pos collision resolving
					throw std::exception("Congratulations! You've triggered the most unlikely error! (penguin was in same position as fishingPenguin pond, please notify Eli)");
				}
				else
				{
					SetState(State::Thinking);
					collider.ResolveDifferentPos(collision);
				}
			}
		}

		//Collide with fishingPenguin
		{
			auto collision = collider.CalculateCollision(fishingPenguin->GetPenguinCollider());
			if (collision.isColliding)
			{
				if (collision.isSamePos)
				{
					//REMOVE by improving same pos collision resolving
					throw std::exception("Congratulations! You've triggered the most unlikely error! (penguin was in same position as fishingPenguin, please notify Eli)");
				}
				else
				{
					SetState(State::Thinking);
					collider.ResolveDifferentPos(collision);
				}
			}
		}
	}

	//Stay within the rink

	//calculation is reduced to only one corner of the rink by using absolute position
	glm::vec2 absPos(abs(pos.x), abs(pos.z));
	//Rink consists of two rectangles and 4 circles making up the corners
	glm::vec2 topRight0(rink.GetRight(), rink.GetTop() - rink.GetCornerRadius());
	glm::vec2 topRight1(rink.GetRight() - rink.GetCornerRadius(), rink.GetTop());

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
		pos += direction * speed * dt;
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
}

void Penguin::UpdateAnimation(float dt)
{
	model->Update(dt);
}

void Penguin::Draw(Camera& camera)
{
	model->AddToRenderQueue(camera);
	for (auto& a : accessories)
	{
		a.Draw(camera);
	}
}

glm::vec3 Penguin::GetPos() const
{
	return pos;
}

const AnimatedModel& Penguin::GetModel() const
{
	return *model;
}

CircleCollider& Penguin::GetCollider()
{
	return collider;
}

void Penguin::InitModel()
{
	model = std::make_unique<AnimatedModel>("Goopie.gltf", transform, "Waddle");
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