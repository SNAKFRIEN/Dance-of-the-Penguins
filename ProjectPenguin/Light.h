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
	unsigned int GetFBO() const;
	unsigned int GetShadowMapTexture() const;
	glm::mat4 GetLightTransform() const;
	const Shader& GetAnimationShader() const;
	const Shader& GetNonAnimationShader() const;
private:
	glm::mat4 CalculateLightTransform(glm::vec3 pos) const;
private:
	//REPLACE: add setting to change resolution?
	static constexpr int shadowResolutionX = 2048;
	static constexpr int shadowResolutionY = 2048;

	static constexpr float nearPlane = 1.0f;
	static constexpr float farPlane = 10.0f;

	Shader nonAnimationShader;
	Shader animationShader;

	unsigned int depthMapFBO;
	unsigned int depthMapTexture;

	const glm::mat4 lightTransform;
};