#pragma once

#include "glm/glm.hpp"

#include "Shader.h"

class Light
{
public:
	Light(glm::vec3 pos, unsigned int shadowResolution);

	void UseNonAnimationShader() const;
	void UseAnimationShader() const;
	void UseBakeTexture() const;
	void UseNonBakeTexture() const;

	int GetShadowResolutionX() const;
	int GetShadowResolutionY() const;
	float GetFarPlane() const;
	unsigned int GetFBO() const;
	unsigned int GetShadowCubeMap() const;
	unsigned int GetBakedShadowCubeMap() const;
	glm::vec3 GetPos() const;
	std::vector<glm::mat4> GetShadowMatrices() const;
	const Shader& GetAnimationShader() const;
	const Shader& GetNonAnimationShader() const;
private:
	std::vector<glm::mat4> CalculateLightTransform(glm::vec3 pos) const;
private:
	const unsigned int shadowResolutionX = 512;
	const unsigned int shadowResolutionY = 512;

	static constexpr float nearPlane = 1.0f;
	static constexpr float farPlane = 80.0f;

	Shader nonAnimationShader;
	Shader animationShader;

	unsigned int depthMapFBO;
	unsigned int depthCubeMap;
	unsigned int depthCubeMapBaked;

	const glm::vec3 pos;
	const std::vector<glm::mat4> lightTransform;
};