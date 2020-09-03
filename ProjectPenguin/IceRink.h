#pragma once

#include "glm/glm.hpp"
#include "Model.h"
#include "ModelNoTexture.h"

//REMOVE, only used for debugging:
#include "Input.h"

class Camera;

//This class represents the physical and visual properties of the ice rink.
//The rink will always be located exactly at the center
class IceRink
{
public:
	IceRink(bool initModels = true);
	
	void Draw(Camera& camera, Input& input);

	float GetRight() const;
	float GetTop() const;
	float GetCornerRadius() const;

	void SetIcePos(glm::vec3 newPos);
private:
	void InitModels();
private:
	//Dimensions
	static constexpr float right = 24.6f;
	static constexpr float top = 13.7f;
	static constexpr float cornerRadius = 5.5f;

	glm::mat4 transform;
	glm::mat4 iceTransform;

	std::unique_ptr<Model> stadiumModel;
	std::unique_ptr<Model> iceModel;
	std::vector<Model> seats;
	std::vector<glm::mat4> seatTransforms;
};