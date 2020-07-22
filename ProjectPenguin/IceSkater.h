#pragma once

#include "glm/glm.hpp"
#include "AnimatedModel.h"

class Camera;
class Input;

class IceSkater
{
public:
	IceSkater(glm::vec3 pos);

	void Update(float dt, const Input& input);
	void Draw(Camera& camera);

	glm::vec3 GetPos() const;
	glm::vec3 GetForward() const;
private:
	glm::vec3 pos;

	float speed = 7.5f;
	float rotationSpeed = 150.0f;

	AnimatedModel model;

	glm::mat4 rotation;
	glm::mat4 transform;
};