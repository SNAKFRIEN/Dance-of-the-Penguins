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
	
	void DrawStatic(Camera& camera, Input& input);
	void DrawNonStatic(Camera& camera, Input& input);
	void Reset();

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
	static constexpr glm::vec3 defaultIcePos = glm::vec3(0.0f, 0.0f, top + 5.0f);

	std::unique_ptr<Model> stadiumModel;
	std::unique_ptr<Model> iceModel;
	std::unique_ptr<Model> iceHole;
	
	std::unique_ptr<Model> seatsRight;
	std::unique_ptr<Model> seatsLeft;
	std::unique_ptr<Model> seatsTopRight;
	std::unique_ptr<Model> seatsTopLeft;
	std::unique_ptr<Model> seatsBottomRight;
	std::unique_ptr<Model> seatsBottomLeft;
};