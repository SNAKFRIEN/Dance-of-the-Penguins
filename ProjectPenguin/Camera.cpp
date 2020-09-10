#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"

void Camera::SetPos(glm::vec3 newPos)
{
	pos = newPos;
}

glm::vec3 Camera::GetPos() const
{
	return pos;
}

void Camera::Follow(glm::vec3 target)
{
	glm::vec3 newPos = target + offset;
	pos = glm::mix(pos, newPos, 0.03f);
	view = glm::lookAt(pos, target, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::SetAspectRatio(float ratio)
{
	projection = glm::perspective(fov, ratio, nearPlane, farPlane);
}

void Camera::CalculateVPMatrix()
{
	viewProjection = projection * view;
}

glm::mat4 Camera::GetVPMatrix() const
{
	return viewProjection;
}

float Camera::GetFOVRadians() const
{
	return fov;
}
