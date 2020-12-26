#pragma once

/*REPLACE: could have done some kind of instancing for this class.
Storing the same texture, vbo, vao, ebo, etc. for each instance is not necessary*/
//REPLACE: if not instancing, at least fix memory leak that's likely present


#include "glm/glm.hpp"

#include "Shader.h"

#include <memory>

class Camera;

class PenguinWarning
{
public:
	PenguinWarning();
	static void PreLoad();

	void Update(glm::vec3 penguinPos, const Camera& camera, glm::vec2 windowDimensions);
	void UpdateWidth(float width);

	static void BindGraphics();
	void Draw();

	float GetHeight() const;

	void SetColor(bool isRed);	//Toggle between yellow and red warning
private:
	bool isRedWarning = false;	//Toggle between yellow and red warning

	//Dimensions
	glm::vec2 pos;
	static constexpr float height = 0.05f;	//Height relative to height of window
	float width = 0.05f;	//Width depends on window aspect ratio
	static constexpr float yMinBorderDistance = 0.1f;	//minimum distance from window border (icon doesn't leave edge of screen)
	static constexpr float yOffset = 1.0f;	//Y offset from penguin position (so it doesn't appear at its feet)

	//Graphics
	static bool preloaded;	//Must be true before an object of this class is created

	//Geometry
	static unsigned int vao;
	static unsigned int vbo;
	static unsigned int ebo;

	//Shader
	static std::unique_ptr<Shader> shader;

	//Texture
	static unsigned int redTexture;
	static unsigned int yellowTexture;
};