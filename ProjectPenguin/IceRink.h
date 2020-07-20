#pragma once

#include "glm/glm.hpp"
#include "Model.h"
#include "ModelNoTexture.h"

class Camera;

class IceRink
{
public:
	IceRink();
	void Draw(Camera& camera);
private:
	//Dimensions
	float top = 16.205f;
	float bottom = -12.293f;
	float left = -24.005f;
	float right = 24.005f;

	glm::mat4 transform;

	Model stadiumModel;
	Model iceModel;
};