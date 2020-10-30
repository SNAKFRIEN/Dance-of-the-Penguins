#include "HomingPenguin.h"

#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"

#include "IceSkater.h"
#include "Collectible.h"
#include "IceRink.h"

HomingPenguin::HomingPenguin(glm::vec3 inPos)
	:
	rng(std::random_device()()),
	randomSwerveTime(0.5f, 5.0f),
	randomRotation(0.0f, 360.0f),
	pos(inPos),
	rotation(randomRotation(rng)),
	model("Goopie.gltf", transform, "Skating"),
	collider(pos, collisionRadius),
	flowerScanner(pos, scanRadius),
	leftWallScanner(leftWallScannerPos, wallScannerRadii),
	rightWallScanner(rightWallScannerPos, wallScannerRadii)
{
	swerveTimer = randomSwerveTime(rng);
}

HomingPenguin::HomingPenguin(const HomingPenguin& rhs)
	:
	rng(std::random_device()()),
	randomSwerveTime(0.5f, 5.0f),
	randomRotation(0.0f, 360.0f),
	pos(rhs.pos),
	rotation(rhs.rotation),
	model("Goopie.gltf", transform, "Skating"),
	collider(pos, collisionRadius),
	flowerScanner(pos, scanRadius),
	leftWallScanner(leftWallScannerPos, wallScannerRadii),
	rightWallScanner(rightWallScannerPos, wallScannerRadii),
	swerveTimer(rhs.swerveTimer)
{
}

HomingPenguin HomingPenguin::operator=(const HomingPenguin& rhs)
{
	pos = rhs.pos;
	rotation = rhs.rotation;
	transform = rhs.transform;
	state = rhs.state;
	speed = rhs.speed;
	rotationSpeed = rhs.speed;
	swerveTimer = rhs.swerveTimer;
	roamingDirection = rhs.roamingDirection;
	return *this;
}

HomingPenguin::HomingPenguin(HomingPenguin&& rhs) noexcept
	:
	rng(std::random_device()()),
	randomSwerveTime(0.5f, 5.0f),
	randomRotation(0.0f, 360.0f),
	pos(rhs.pos),
	rotation(rhs.rotation),
	model("Goopie.gltf", transform, "Skating"),
	collider(pos, collisionRadius),
	flowerScanner(pos, scanRadius),
	leftWallScanner(leftWallScannerPos, wallScannerRadii),
	rightWallScanner(rightWallScannerPos, wallScannerRadii)
{
}

void HomingPenguin::Update(IceSkater& player, std::vector<Collectible>& flowers, const IceRink& rink, float dt)
{
	switch (state)
	{
	case State::Roaming:
		//Steer away from obstacles
		leftWallScannerPos = pos + glm::rotate(leftWallScannerBasePos, rotation, glm::vec3(0.0f, -1.0f, 0.0f));
		rightWallScannerPos = pos + glm::rotate(rightWallScannerBasePos, rotation, glm::vec3(0.0f, -1.0f, 0.0f));
		if (!rightWallScanner.IsInRink(rink))
		{
			std::cout << "right wall!!!" << std::endl;
			speed = wallAvoidSpeed;
			roamingDirection = RoamingDirection::Left;
			swerveTimer = randomSwerveTime(rng);	//Reset swerve timer
		}
		else if(!leftWallScanner.IsInRink(rink))
		{
			std::cout << "left wall!!!" << std::endl;
			speed = wallAvoidSpeed;
			roamingDirection = RoamingDirection::Right;
			swerveTimer = randomSwerveTime(rng);	//Reset swerve timerr
		}
		else
		{
			speed = baseSpeed;
			//Check whether its time to turn in a different direction
			swerveTimer -= dt;
			if (swerveTimer <= 0.0f)
			{
				//Reset timer
				swerveTimer += randomSwerveTime(rng);

				//Change direction
				if (roamingDirection == RoamingDirection::Left)
				{
					roamingDirection = RoamingDirection::Right;
				}
				else
				{
					roamingDirection = RoamingDirection::Left;
				}
			}
		}

		//Update position
		if (roamingDirection == RoamingDirection::Left)
		{
			rotation -= glm::radians(rotationSpeed * dt);
		}
		else
		{
			rotation += glm::radians(rotationSpeed * dt);
		}

		pos += glm::rotate(glm::vec3(0.0f, 0.0f, -1.0f), rotation, glm::vec3(0.0f, -1.0f, 0.0f)) * speed * dt;
		transform = glm::translate(glm::mat4(1.0f), pos) * glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, -1.0f, 0.0f));

		//Scan for flowers
		for(int i = 0; i < flowers.size() && state == State::Roaming; i++)
		{
			if (flowerScanner.CalculateCollision(flowers[i].GetCollider()).isColliding)
			{
				state = State::HomingFlower;
			}
		}

		break;
	case State::HomingFlower:
	{
		//Steer away from obstacles
		leftWallScannerPos = pos + glm::rotate(leftWallScannerBasePos, rotation, glm::vec3(0.0f, -1.0f, 0.0f));
		rightWallScannerPos = pos + glm::rotate(rightWallScannerBasePos, rotation, glm::vec3(0.0f, -1.0f, 0.0f));
		if (!rightWallScanner.IsInRink(rink))
		{
			std::cout << "right wall!!!" << std::endl;
			speed = wallAvoidSpeed;
			rotation -= glm::radians(rotationSpeed * dt);
		}
		else if (!leftWallScanner.IsInRink(rink))
		{
			std::cout << "left wall!!!" << std::endl;
			speed = wallAvoidSpeed;
			rotation += glm::radians(rotationSpeed * dt);
		}
		else
		{
			std::cout << "Homing" << std::endl;
			speed = baseSpeed;
			//Find closest flower
			CollisionData closestFlower;
			closestFlower.isColliding = false;
			closestFlower.distanceSquared = INFINITY;
			for (Collectible& flower : flowers)
			{
				auto temp = flowerScanner.CalculateCollision(flower.GetCollider());
				if (temp.isColliding && temp.distanceSquared < closestFlower.distanceSquared)
				{
					closestFlower = temp;
				}
			}

			//Check that a flower was found
			if (closestFlower.isColliding == false)
			{
				state = State::Roaming;
			}
			else
			{
				//Steer towards target flower
				glm::vec3 direction = glm::normalize(closestFlower.colliderB->GetPos() - pos);
				float angle = glm::orientedAngle(direction, GetForward(), glm::vec3(0.0f, -1.0f, 0.0f));
				if (angle > 0.0f)
				{
					rotation += std::min(angle, glm::radians(rotationSpeed * dt));
				}
				else if (angle < 0.0f)
				{
					rotation += std::max(angle, glm::radians(-rotationSpeed * dt));
				}
			}
		}
		pos += glm::rotate(glm::vec3(0.0f, 0.0f, -1.0f), rotation, glm::vec3(0.0f, -1.0f, 0.0f)) * speed * dt;
		transform = glm::translate(glm::mat4(1.0f), pos) * glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, -1.0f, 0.0f));
	}
		break;
	case State::HomingPlayer:
		glm::vec3 direction = glm::normalize(player.GetPos() - pos);
		float angle = glm::orientedAngle(direction, GetForward(), glm::vec3(0.0f, -1.0f, 0.0f));
		if (angle > 0.0f)
		{
			rotation += std::min(angle, glm::radians(rotationSpeed * dt));
		}
		else if (angle < 0.0f)
		{
			rotation += std::max(angle, glm::radians(-rotationSpeed * dt));
		}
		pos += glm::rotate(glm::vec3(0.0f, 0.0f, -1.0f), rotation, glm::vec3(0.0f, -1.0f, 0.0f)) * speed * dt;
		transform = glm::translate(glm::mat4(1.0f), pos) * glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, -1.0f, 0.0f));
		break;
	}
}

void HomingPenguin::UpdateAnimation(float dt)
{
	model.Update(dt);
}

void HomingPenguin::Draw(Camera& camera)
{
	model.AddToRenderQueue(camera);
}

void HomingPenguin::GiveFlower()
{
	speed = playerHomingSpeed;
	rotationSpeed = playerHomingRotationSpeed;
	state = State::HomingPlayer;
}

CircleCollider& HomingPenguin::GetCollider()
{
	return collider;
}

glm::vec3 HomingPenguin::GetForward() const
{
	return glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, -1.0f, 0.0f))[2];
}
