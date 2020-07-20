#include "IceSkater.h"

#include "Input.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"

//REMOVE, only used for debug brake
#include "GLFW/glfw3.h"

IceSkater::IceSkater(glm::vec3 pos)
	:
	pos(pos),
	model("IceSkater.gltf", transform)
{
	rotation = glm::mat4(1.0f);
}

void IceSkater::Update(float dt, const Input& input)
{
	bool pressed = false;
	glm::vec3 direction(0.0f);
	if (input.IsPressed(InputAction::Forward))
	{
		direction.z = -1.0f;
		pressed = true;
	}
	if (input.IsPressed(InputAction::Backward))
	{
		direction.z = 1.0f;
		pressed = true;
	}
	if (input.IsPressed(InputAction::Right))
	{
		direction.x = 1.0f;
		pressed = true;
	}
	if (input.IsPressed(InputAction::Left))
	{
		direction.x = -1.0f;
		pressed = true;
	}

	float deltaRotation = 0.0f;
	if (pressed)
	{
		direction = glm::normalize(direction);
		float angle = glm::orientedAngle(direction, GetForward(), glm::vec3(0.0f, -1.0f, 0.0f));
		if (angle > 0.0f)
		{
			deltaRotation = std::min(angle, glm::radians(1.5f));
		}
		else if (angle < 0.0f)
		{
			deltaRotation = std::max(angle, glm::radians(-1.5f));
		}
		rotation = glm::rotate(rotation, deltaRotation, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	if (input.IsPressed(GLFW_KEY_SPACE))
	{
		pos += GetForward() * speed;
	}

	transform = glm::translate(glm::mat4(1.0f), pos) * rotation;
}

void IceSkater::Draw(Camera& camera)
{
	model.Draw(camera);
}

glm::vec3 IceSkater::GetPos() const
{
	return pos;
}

glm::vec3 IceSkater::GetForward() const
{
	return rotation[2];
}
