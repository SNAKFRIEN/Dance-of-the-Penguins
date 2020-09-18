#pragma once

#include "glm/glm.hpp"

class Camera
{
public:
	void SetPos(glm::vec3 newPos);
	glm::vec3 GetPos() const;

	void Follow(glm::vec3 target);
	void LookAt(glm::vec3 eye, glm::vec3 target, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));

	void SetAspectRatio(float ratio); //This function will automatically be called whenever the window is resized

	void CalculateVPMatrix();

	glm::mat4 GetVPMatrix() const;
	float GetFOVRadians() const;
private:
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 viewProjection;

	static constexpr float fov = glm::radians(45.0f);
	static constexpr float nearPlane = 0.1f;
	static constexpr float farPlane = 200.0f;

	//Gameplay properties
	glm::vec3 pos = glm::vec3(0.0f);
	static constexpr glm::vec3 offset = glm::vec3(0.0f, 12.0f, 12.0f);
};