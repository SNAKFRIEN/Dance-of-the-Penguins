#pragma once

//This class could've supported many different animations, but we only ever needed the smoke animation

#include "glm/glm.hpp"

#include "Shader.h"

#include <memory>

class Camera;

class SmokeEffect
{
public:
	SmokeEffect(glm::vec3 pos);
	static void PreLoad();
	
	void Update(float deltaTime);
	static void BindGraphics();
	void Draw(const Camera& camera);

	bool IsFinished() const;
private:
	static bool preloaded;	//Must be true before an object of this class is created
	static unsigned int vao;
	static unsigned int vbo;
	static unsigned int ebo;
	static std::unique_ptr<Shader> shader;
	static unsigned int texture;

	float currentTime = 0.0f;
	int currentFrame = 0;
	static constexpr float timePerFrame = 0.075f;
	bool finished = false;
	static constexpr int nFrames = 9;
	static constexpr float frameWidth = 1.0f / (float)nFrames;

	glm::mat4 translation;
	glm::vec3 pos;
};