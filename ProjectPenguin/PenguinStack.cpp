#include "PenguinStack.h"

#include "glm/gtx/rotate_vector.hpp"

PenguinStack::PenguinStack(glm::vec3 inPos, glm::vec3 target, std::mt19937& rng, AudioManager& audioManager)
	:
	model("Goopie.gltf", transform),
	collider(pos, collisionRadius),
	pos(inPos),
	direction(glm::normalize(target - pos)),
	rng(rng),
	randomNStackedPenguins(2, 4)
{
	//Set animation
	model.SetAnimation("GoombaBottomSmooth");
	//Recursively populate stack
	int nStackedPenguins = randomNStackedPenguins(rng) - 1;	//Subtract 1 because bottom penguin already exists
	firstNode = std::make_unique<PenguinNode>(model, nStackedPenguins);
}

void PenguinStack::Update(float dt)
{
	pos += direction * speed * dt;
	//Construct transform from pos and direction
	transform = glm::translate(glm::mat4(1.0f), pos) * glm::orientation(direction, glm::vec3(0.0f, 0.0f, -1.0f));
}

void PenguinStack::UpdateAnimation(float dt)
{
	model.Update(dt);
	firstNode->UpdateAnimation(dt);
}

void PenguinStack::Draw(Camera& camera)
{
	model.AddToRenderQueue(camera);
	firstNode->Draw(camera);
}

glm::vec3 PenguinStack::GetPos() const
{
	return pos;
}

CircleCollider& PenguinStack::GetCollider()
{
	return collider;
}

PenguinStack::PenguinNode::PenguinNode(const AnimatedModel& parentModel, int nStackedRemaining)
	:
	model("Goopie.gltf", parentModel, "head")
{
	//Set animation
	model.SetAnimation("GoombaTopSmooth");
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
