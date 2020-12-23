#include "HomingPenguin.h"

#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"

#include <iostream>

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
	rightWallScanner(rightWallScannerPos, wallScannerRadii),
	hat("TrafficConeHat.gltf", model, "head"),
	vest("SecurityVest.gltf", model, "torso")
{
	swerveTimer = randomSwerveTime(rng);
	std::cout << "HomingPenguin constructed" << std::endl;
}

HomingPenguin::HomingPenguin(const HomingPenguin& rhs)
	:
	rng(std::random_device()()),
	randomSwerveTime(0.5f, 5.0f),
	randomRotation(0.0f, 360.0f),

	state(rhs.state),
	roamingDirection(rhs.roamingDirection),

	pos(rhs.pos),
	rotation(rhs.rotation),
	//REPLACE: copy transform?

	speed(rhs.speed),
	rotationSpeed(rhs.rotationSpeed),

	fallingTime(rhs.fallingTime),

	leftWallScanner(leftWallScannerPos, wallScannerRadii),
	rightWallScanner(rightWallScannerPos, wallScannerRadii),

	flowerScanner(pos, scanRadius),

	model("Goopie.gltf", transform, rhs.model.GetAnimation()),
	hat("TrafficConeHat.gltf", model, "head"),
	vest("SecurityVest.gltf", model, "torso"),

	collider(pos, collisionRadius),

	finished(rhs.finished)
{
	model.SetCurrentAnimationTime(rhs.model.GetCurrentAnimationTime());

	if (rhs.flower.get())
	{
		GiveFlower();
	}
	else
	{
		flower.release();
	}

	std::cout << "HomingPenguin copy constructed" << std::endl;
}

HomingPenguin HomingPenguin::operator=(const HomingPenguin& rhs)
{
	std::cout << "HomingPenguin copied" << std::endl;
	
	swerveTimer = rhs.swerveTimer;

	state = rhs.state;
	roamingDirection = rhs.roamingDirection;

	pos = rhs.pos;
	rotation = rhs.rotation;
	//REPLACE: copy transform?

	speed = rhs.speed;
	rotationSpeed = rhs.rotationSpeed;

	fallingTime = rhs.fallingTime;

	model.SetAnimation(rhs.model.GetAnimation());
	model.SetCurrentAnimationTime(rhs.model.GetCurrentAnimationTime());

	finished = rhs.finished;

	if (rhs.flower.get())
	{
		GiveFlower();
	}
	else
	{
		flower.release();
	}

	return *this;
}

HomingPenguin::HomingPenguin(HomingPenguin&& rhs) noexcept
	:
	rng(std::random_device()()),
	randomSwerveTime(0.5f, 5.0f),
	randomRotation(0.0f, 360.0f),

	state(rhs.state),
	roamingDirection(rhs.roamingDirection),

	pos(rhs.pos),
	rotation(rhs.rotation),
	//REPLACE: copy transform?

	speed(rhs.speed),
	rotationSpeed(rhs.rotationSpeed),

	fallingTime(rhs.fallingTime),

	leftWallScanner(leftWallScannerPos, wallScannerRadii),
	rightWallScanner(rightWallScannerPos, wallScannerRadii),

	flowerScanner(pos, scanRadius),

	model("Goopie.gltf", transform, rhs.model.GetAnimation()),
	hat("TrafficConeHat.gltf", model, "head"),
	vest("SecurityVest.gltf", model, "torso"),

	collider(pos, collisionRadius),

	finished(rhs.finished)
{
	model.SetCurrentAnimationTime(rhs.model.GetCurrentAnimationTime());

	if (rhs.flower.get())
	{
		GiveFlower();
	}
	else
	{
		flower.release();
	}

	std::cout << "HomingPenguin move constructed" << std::endl;
}

HomingPenguin HomingPenguin::operator=(HomingPenguin&& rhs) noexcept
{
	std::cout << "HomingPenguin moved" << std::endl;

	swerveTimer = rhs.swerveTimer;

	state = rhs.state;
	roamingDirection = rhs.roamingDirection;

	pos = rhs.pos;
	rotation = rhs.rotation;
	//REPLACE: copy transform?

	speed = rhs.speed;
	rotationSpeed = rhs.rotationSpeed;

	fallingTime = rhs.fallingTime;

	model.SetAnimation(rhs.model.GetAnimation());
	model.SetCurrentAnimationTime(rhs.model.GetCurrentAnimationTime());

	finished = rhs.finished;

	if (rhs.flower.get())
	{
		GiveFlower();
	}
	else
	{
		flower.release();
	}

	return *this;
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
			speed = wallAvoidSpeed;
			roamingDirection = RoamingDirection::Left;
			swerveTimer = randomSwerveTime(rng);	//Reset swerve timer
		}
		else if(!leftWallScanner.IsInRink(rink))
		{
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
			speed = wallAvoidSpeed;
			rotation -= glm::radians(rotationSpeed * dt);
		}
		else if (!leftWallScanner.IsInRink(rink))
		{
			speed = wallAvoidSpeed;
			rotation += glm::radians(rotationSpeed * dt);
		}
		else
		{
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
	{
		if (speed < playerHomingSpeed)
		{
			speed += playerHomingAcceleration * dt;
		}
		else
		{
			speed = playerHomingSpeed;
		}

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
	}
		break;
	case State::Crashing:
		//Uses similar code to penguin stack
		//Curve is based on fallingTime squared
		fallingTime += dt;
		if (fallingTime >= 2.0f)
		{
			finished = true;	//The homingPenguin has landed and can be removed from the game
		}
		
		//Calculate curve
		float t = fallingTime * fallingSpeed;
		float x = t;
		float y = -pow(t - 1.0f, 2.0f) + 1.0f; //Curve goes through (0, 0), up to (1, 1) and back down to (2, 0)

		//Transform curve to current position
		glm::vec3 curvePos = glm::vec3(0.0f, y, -x) * fallingCurveScale;	//curvePos = current position in the curve
		transform = glm::translate(glm::mat4(1.0f), pos)
			* glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, -1.0f, 0.0f))
			* glm::translate(glm::mat4(1.0f), curvePos)
			* glm::rotate(glm::mat4(1.0f), fallingTime * fallingFlipSpeed, glm::vec3(-1.0f, 0.0f, 0.0f));
		break;
	}
}

void HomingPenguin::UpdateAnimation(float dt)
{
	model.Update(dt);
}

void HomingPenguin::Draw(Camera& camera)
{
	if (flower.get())
	{
		flower->Draw(camera);
	}
	model.AddToRenderQueue(camera);
	hat.Draw(camera);
	vest.Draw(camera);
}

void HomingPenguin::GiveFlower()
{
	rotationSpeed = playerHomingRotationSpeed;
	flower = std::make_unique<JointAttachment>("BouquetGoop.gltf", model, "lower_arm.R");
	state = State::HomingPlayer;
	model.SetAnimation("SkatingWhileHolding");
}

void HomingPenguin::Collide(const IceRink& rink)
{
	if (!collider.IsInRink(rink))
	{
		state = State::Crashing;
		model.SetAnimation("GoombaFalling");
	}
}

CircleCollider& HomingPenguin::GetCollider()
{
	return collider;
}

glm::vec3 HomingPenguin::GetForward() const
{
	return glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, -1.0f, 0.0f))[2];
}

glm::vec3 HomingPenguin::GetPos() const
{
	return transform[3];
}

bool HomingPenguin::IsLockedOn() const
{
	return state == State::HomingPlayer;
}

bool HomingPenguin::IsFinished() const
{
	return finished;
}
