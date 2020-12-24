#include "PenguinStack.h"

#include "glm/gtx/rotate_vector.hpp"

#include "SmokeMachine.h"

PenguinStack::PenguinStack(glm::vec3 inPos, glm::vec3 target, std::mt19937& rng, AudioManager& audioManager)
	:
	model("Goopie.gltf", transform, "GoombaBottomSmooth"),
	collider(pos, collisionRadius),
	pos(inPos),
	direction(glm::normalize(target - pos)),
	rotationMatrix(glm::orientation(direction, glm::vec3(0.0f, 0.0f, -1.0f))),
	rng(rng),
	randomNStackedPenguins(3, 5),
	randomFallingSpeed(0.8f, 1.5f),
	randomFallingRotationOffset(glm::radians(-45.0f), glm::radians(45.0f)),
	randomFallingDistanceMultiplier(4.0f, 8.0f),
	randomFallingCurveHeight(2.0f, 6.0f),
	randomFallingFlipSpeed(glm::radians(90.0f), glm::radians(360.0f))
{
	//Recursively populate stack
	nPenguins = randomNStackedPenguins(rng);
	int nStackedPenguins =  nPenguins - 1;	//Subtract 1 because bottom penguin already exists
	firstNode = std::make_unique<PenguinNode>(model, nStackedPenguins);
}

void PenguinStack::Update(float dt, const IceRink& rink, SmokeMachine& smokeMachine)
{
	switch (state)
	{
	case State::Skating:
		pos += direction * speed * dt;
		//Construct transform from pos and direction
		transform = glm::translate(glm::mat4(1.0f), pos) * rotationMatrix;
		if (!collider.IsInRink(rink))
		{
			Crash(smokeMachine);
			Update(0.0f, rink, smokeMachine);
		}
		break;
	case State::Crashing:
		//Curve is based on elapsedTime squared
		fallingTime += dt;
		for (FallingPenguin& fp : fallingPenguins)
		{
			//Calculate curve
			float t = fallingTime * fp.speed;
			float x = t;
			float y = -pow(t - 1.0f, 2.0f) + 1.0f; //Curve goes through (0, 0), up to (1, 1) and back down to (2, 0)

			//Apply curve to transform
			glm::vec3 curveScale(0.0f, fp.height, fp.distanceMultiplier);
			glm::vec3 curvePos = glm::vec3(0.0f, y, -x) * curveScale;
			curvePos = glm::rotateY(curvePos, fp.rotationOffset);
			fp.transform = transform
				* glm::translate(glm::mat4(1.0f), curvePos)
				* glm::rotate(glm::mat4(1.0f), fallingTime * fp.flipSpeed, glm::vec3(-1.0f, 0.0f, 0.0f));
		}
		break;
	}
}

void PenguinStack::UpdateAnimation(float dt)
{
	switch (state)
	{
	case State::Skating:
		model.Update(dt);
		firstNode->UpdateAnimation(dt);
		break;
	case State::Crashing:
		for (FallingPenguin& fp : fallingPenguins)
		{
			fp.model.Update(dt);
		}
		break;
	}
}

void PenguinStack::Draw(Camera& camera)
{
	switch (state)
	{
	case State::Skating:
		model.AddToRenderQueue(camera);
		firstNode->Draw(camera);
		break;
	case State::Crashing:
		for (FallingPenguin& fp : fallingPenguins)
		{
			fp.model.AddToRenderQueue(camera);
		}
		break;
	}
}

void PenguinStack::Crash(SmokeMachine& smokeMachine)
{
	smokeMachine.SpawnSmoke(pos);
	state = State::Crashing;
	fallingPenguins.reserve(nPenguins);
	for (int i = 0; i < nPenguins; i++)
	{
		fallingPenguins.emplace_back();
		fallingPenguins[i].speed = randomFallingSpeed(rng);
		fallingPenguins[i].rotationOffset = randomFallingRotationOffset(rng);
		fallingPenguins[i].distanceMultiplier = randomFallingDistanceMultiplier(rng);
		fallingPenguins[i].height = randomFallingCurveHeight(rng);
		fallingPenguins[i].flipSpeed = randomFallingFlipSpeed(rng);
	}
}

glm::vec3 PenguinStack::GetPos() const
{
	return pos;
}

CircleCollider& PenguinStack::GetCollider()
{
	return collider;
}

bool PenguinStack::IsCrashing()
{
    return state == State::Crashing;
}

PenguinStack::PenguinNode::PenguinNode(const AnimatedModel& parentModel, int nStackedRemaining)
	:
	model("Goopie.gltf", parentModel, "head", "GoombaTopSmooth")
{
	//Decrement before creating next node
	nStackedRemaining--;
	if (nStackedRemaining > 0)
	{
		nextNode = std::make_unique<PenguinNode>(model.GetModel(), nStackedRemaining);
	}
}

void PenguinStack::PenguinNode::UpdateAnimation(float dt)
{
	model.UpdateAnimation(dt);
	if (nextNode)
	{
		nextNode->UpdateAnimation(dt);
	}
}

void PenguinStack::PenguinNode::Draw(Camera& camera)
{
	model.Draw(camera);
	if (nextNode)
	{
		nextNode->Draw(camera);
	}
}