#pragma once

/*REPLACE: could have done some kind of instancing for this class.
Storing the same texture, vbo, vao, ebo, etc. for each instance is not necessary*/

#include "glm/glm.hpp"
#include "Shader.h"

class Camera;

class PenguinWarning
{
public:
	PenguinWarning();

	void Update(glm::vec3 penguinPos, const Camera& camera);
	void UpdateWidth(float width);

	void Draw();

	float GetHeight() const;
private:
	//Dimensions
	glm::vec2 pos;
	static constexpr float height = 0.05f;	//Height relative to height of window
	float width = 0.05f;	//Width depends on window aspect ratio
	static constexpr float minBorderDistance = 0.05f;

	//Geometry
	unsigned int vao = 0;
	unsigned int vbo;
	unsigned int ebo;

	//Shader
	Shader shader;

	//Texture
	unsigned int texture = 0;
};