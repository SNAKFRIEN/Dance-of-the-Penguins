#pragma once

#include "glm/glm.hpp"
#include "Model.h"
#include "ModelNoTexture.h"

//REMOVE, only used for debugging:
#include "Input.h"

class Camera;
class Collectible;

//This class represents the physical and visual properties of the ice rink.
//The rink will always be located exactly at the center
class IceRink
{
public:
	IceRink(bool initModels = true);

	//REMOVE input parameters! these are exclusively used for debugging
	void DrawStatic(Camera& camera, Input& input);
	void DrawNonStatic(Camera& camera, Input& input, const std::vector<glm::vec3>& flowerPositions);
	void Reset();
	void Update(float elapsedTime);	//Makes the audience... Wiggle...

	float GetRight() const;
	float GetTop() const;
	float GetCornerRadius() const;

	void SetIcePos(glm::vec3 newPos);

	void SetAudienceBlock1(bool isPenguin);
	void SetAudienceBlock2(bool isPenguin);
	void SetAudienceBlock3(bool isPenguin);
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

	glm::mat4 block1Transform;
	glm::mat4 block2Transform;
	glm::mat4 block3Transform;
	glm::mat4 block4Transform;
	std::unique_ptr<Model> audienceBlock1;
	std::unique_ptr<Model> audienceBlock2;
	std::unique_ptr<Model> audienceBlock3;
	std::unique_ptr<Model> audienceBlock4;
	std::unique_ptr<Model> penguinBlock1;
	std::unique_ptr<Model> penguinBlock2;
	std::unique_ptr<Model> penguinBlock3;
	std::unique_ptr<Model> penguinBlock4;
	bool block1IsPenguin;
	bool block2IsPenguin;
	bool block3IsPenguin;
	bool block4IsPenguin;
	static constexpr float audienceWiggleStrength = 0.1f;
	static constexpr float block1WiggleSpeedMultiplier = 1.0f;
	static constexpr float block2WiggleSpeedMultiplier = 2.0f;
	static constexpr float block3WiggleSpeedMultiplier = 3.0f;
	static constexpr float block4WiggleSpeedMultiplier = 4.0f;
};