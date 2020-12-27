#pragma once

//This class could be much more versatile, but we don't need that

#include "glm/glm.hpp"

#include "Shader.h"

#include <memory>

class Camera;

class Plus5Effect
{
public:
	Plus5Effect(glm::vec3 pos);
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
	static constexpr float maxLifeTime = 0.9f;
	static constexpr float speed = 2.0f;

	glm::vec3 pos;
};