#pragma once

#include "glm/glm.hpp"

#include "Model.h"
#include "AnimatedModel.h"

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
	void Update(float deltaTime);
	void UpdateFerrisWheelAndCarousel(float deltaTime);

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
	std::vector<Model> staticSurroundings;

	//Animations
	std::unique_ptr<Model> ferrisWheel;
	glm::mat4 ferrisWheelTransform;
	std::vector<Model> ferrisWheelCarts;
	std::vector<glm::mat4> ferrisWheelCartTransforms;
	const glm::mat4 ferrisWheelRotationAndTranslationMat;
	float ferrisWheelRotation = 0.0f;
	
	std::unique_ptr<Model> carousel;
	glm::mat4 carouselTransform;
	const glm::mat4 carouselTranslation;
	float carouselRotation = 0.0f;

	//Lights
	std::vector<glm::vec3> lightSources;
	std::vector<glm::vec3> ferrisWheelLightSources;
};