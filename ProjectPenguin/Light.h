#pragma once

#include "glm/glm.hpp"

#include "Shader.h"

class Light
{
public:
	Light(glm::vec3 pos);

	void UseNonAnimationShader() const;
	void UseAnimationShader() const;

	int GetShadowResolutionX() const;
	int GetShadowResolutionY() const;
	float GetFarPlane() const;
	unsigned int GetFBO() const;
	unsigned int GetShadowCubeMap() const;
	glm::vec3 GetPos() const;
	std::vector<glm::mat4> GetShadowMatrices() const;
	const Shader& GetAnimationShader() const;
	const Shader& GetNonAnimationShader() const;
private:
	std::vector<glm::mat4> CalculateLightTransform(glm::vec3 pos) const;
private:
	//REPLACE: add setting to change resolution?
	static constexpr int shadowResolutionX = 2048;
	static constexpr int shadowResolutionY = 2048;

	static constexpr float nearPlane = 1.0f;
	static constexpr float farPlane = 80.0f;

	Shader nonAnimationShader;
	Shader animationShader;

	unsigned int depthMapFBO;
	unsigned int depthCubeMap;

	const glm::vec3 pos;
	const std::vector<glm::mat4> lightTransform;
};