#pragma once

#include "glm/glm.hpp"

class Camera
{
public:
	void SetPos(glm::vec3 newPos);
	glm::vec3 GetPos() const;

	void Follow(glm::vec3 target);

	void SetAspectRatio(float ratio); //This function will automatically be called whenever the window is resized

	glm::mat4 GetVPMatrix() const;
	float GetFOVRadians() const;
private:
	glm::mat4 view;
	glm::mat4 projection;

	static constexpr float fov = glm::radians(45.0f);
	static constexpr float nearPlane = 0.1f;
	static constexpr float farPlane = 200.0f;

	//Gameplay properties
	glm::vec3 pos = glm::vec3(0.0f);
	static constexpr glm::vec3 offset = glm::vec3(0.0f, 13.0f, 13.0f);
};