#include "Light.h"

#include "glad/glad.h"
#include "glm/gtc/matrix_transform.hpp"

Light::Light(glm::vec3 pos)
	:
	nonAnimationShader("DepthOnly.vert", "DepthOnly.frag"),
	animationShader("DepthOnlyAnimation.vert", "DepthOnly.frag"),
	lightTransform(CalculateLightTransform(pos))
{
	//Create depth map FBO
	glGenFramebuffers(1, &depthMapFBO);

	//Create depth map texture
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowResolutionX, shadowResolutionY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Attach depth map texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Light::UseNonAnimationShader() const
{
	nonAnimationShader.Use();
}

void Light::UseAnimationShader() const
{
	animationShader.Use();
}

int Light::GetShadowResolutionX() const
{
	return shadowResolutionX;
}

int Light::GetShadowResolutionY() const
{
	return shadowResolutionY;
}

unsigned int Light::GetFBO() const
{
	return depthMapFBO;
}

unsigned int Light::GetShadowMapTexture() const
{
	return depthMapTexture;
}

glm::mat4 Light::GetLightTransform() const
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

glm::mat4 Light::CalculateLightTransform(glm::vec3 pos) const
{
	//perspective * view matrix
	auto persp = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);
	auto view = glm::lookAt(pos, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	return persp * view;
}
