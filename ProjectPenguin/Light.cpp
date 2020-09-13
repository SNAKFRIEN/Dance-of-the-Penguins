#include "Light.h"

#include "glad/glad.h"
#include "glm/gtc/matrix_transform.hpp"

Light::Light(glm::vec3 pos, unsigned int shadowResolution)
	:
	pos(pos),
	shadowResolutionX(shadowResolution),
	shadowResolutionY(shadowResolution),
	nonAnimationShader("DepthOnly.vert", "DepthOnly.frag", "DepthOnly.geom"),
	animationShader("DepthOnlyAnimation.vert", "DepthOnly.frag", "DepthOnly.geom"),
	lightTransform(CalculateLightTransform(pos))
{
	//Create depth map FBO
	glGenFramebuffers(1, &depthMapFBO);

	//Create depth cubemap
	glGenTextures(1, &depthCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i
			, 0
			, GL_DEPTH_COMPONENT
			, shadowResolutionX
			, shadowResolutionY
			, 0
			, GL_DEPTH_COMPONENT
			, GL_FLOAT
			, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//Create baked depth cubemap REPLACE: hardcode shadowRes? If yes, REPLACE GetShadowRes() to scale viewPort correctly!!!
	glGenTextures(1, &depthCubeMapBaked);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMapBaked);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i
			, 0
			, GL_DEPTH_COMPONENT
			, shadowResolutionX
			, shadowResolutionY
			, 0
			, GL_DEPTH_COMPONENT
			, GL_FLOAT
			, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	/*REMOVE? same code as UseBakeTexture() and UseNonBakeTexture
	//Attach depth map texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	*/
}

void Light::UseNonAnimationShader() const
{
	nonAnimationShader.Use();
}

void Light::UseAnimationShader() const
{
	animationShader.Use();
}

void Light::UseBakeTexture() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMapBaked, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Light::UseNonBakeTexture() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int Light::GetShadowResolutionX() const
{
	return shadowResolutionX;
}

int Light::GetShadowResolutionY() const
{
	return shadowResolutionY;
}

float Light::GetFarPlane() const
{
	return farPlane;
}

unsigned int Light::GetFBO() const
{
	return depthMapFBO;
}

unsigned int Light::GetShadowCubeMap() const
{
	return depthCubeMap;
}

unsigned int Light::GetBakedShadowCubeMap() const
{
	return depthCubeMapBaked;
}

glm::vec3 Light::GetPos() const
{
	return pos;
}

std::vector<glm::mat4> Light::GetShadowMatrices() const
{
	return lightTransform;
}

const Shader& Light::GetAnimationShader() const
{
	return animationShader;
}

const Shader& Light::GetNonAnimationShader() const
{
	return nonAnimationShader;
}

std::vector<glm::mat4> Light::CalculateLightTransform(glm::vec3 pos) const
{
	//Calculate perspective
	float aspectRatio = (float)shadowResolutionX / (float)shadowResolutionY;
	auto persp = glm::perspective(glm::radians(90.0f), aspectRatio, nearPlane, farPlane);

	//Multiply perspective by view matrices
	std::vector<glm::mat4> result;
	result.push_back(persp * glm::lookAt(pos, pos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	result.push_back(persp * glm::lookAt(pos, pos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	result.push_back(persp * glm::lookAt(pos, pos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	result.push_back(persp * glm::lookAt(pos, pos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	result.push_back(persp * glm::lookAt(pos, pos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	result.push_back(persp * glm::lookAt(pos, pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

	return result;
}
