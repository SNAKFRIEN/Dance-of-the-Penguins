#pragma once

#include "glm/glm.hpp"
#include "Model.h"
#include "ModelNoTexture.h"

#include <memory>

class Camera;
class Collectible;

//This class represents the physical and visual properties of the ice rink.
//The rink will always be located exactly at the center
class IceRink
{
public:
	IceRink(bool initModels = true);

	void DrawStatic(Camera& camera);
	void DrawNonStatic(Camera& camera, const std::vector<glm::vec3>& collectiblePositions);
	void Reset();
	void Update(float elapsedTime);

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

	//Ice
	std::unique_ptr<Model> iceModel;
	std::unique_ptr<Model> iceHole;

	//Surroundings
	//REPLACE: might as well keep all of these in one vector?
	std::unique_ptr<Model> ground;
	std::unique_ptr<Model> market;
	std::unique_ptr<Model> lamps;
	std::unique_ptr<Model> trees;
	std::unique_ptr<Model> restaurant;
	std::unique_ptr<Model> mountains;
	std::unique_ptr<Model> backgroundHouses;
	std::unique_ptr<Model> house;
	std::unique_ptr<Model> ferrisWheel;
	std::unique_ptr<Model> blackBox;

	std::vector<glm::vec3> lightSources;
};