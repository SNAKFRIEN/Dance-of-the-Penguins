#pragma once

#include "glm/glm.hpp"
#include "Model.h"
#include "ModelNoTexture.h"

class Camera;

class IceRink
{
public:
	IceRink(bool initModels = true);
	void Draw(Camera& camera);
private:
	void InitModels();
private:
	//Dimensions
	float top = 15.0f;
	float bottom = -15.0f;
	float left = -26.0f;
	float right = 26.0f;
	float cornerRadius = 6.0f;

	glm::mat4 transform;

	std::unique_ptr<Model> stadiumModel;
	std::unique_ptr<Model> iceModel;
};