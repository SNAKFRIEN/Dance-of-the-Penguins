#pragma once

#include "Shader.h"

class Input;

class UIButton
{
public:
	UIButton(float left, float top, float right, float bottom, glm::vec2 relativeTopLeft, glm::vec2 relativeBottomRight, std::string textureName);
	~UIButton();
	UIButton(const UIButton&) = delete;
	UIButton operator=(const UIButton&) = delete;
	UIButton(UIButton&& rhs) noexcept;
	UIButton operator=(UIButton&& rhs) = delete;

	void UpdateSize(float newLeft, float newTop, float newRight, float newBottom);
	bool UpdateAndCheckClick(const Input& input);
	void Draw();

	void SetOnColor(glm::vec3 newColor);
	void SetOffColor(glm::vec3 newColor);

	glm::vec2 GetRelativeTopLeft() const;
	glm::vec2 GetRelativeBottomRight() const;
	float GetLeft() const;
	float GetRight() const;
	float GetTop() const;
	float GetBottom() const;
private:
	//Geometry
	unsigned int vao = 0;
	unsigned int vbo;
	unsigned int ebo;

	//Shader
	Shader shader;

	//Texture
	unsigned int texture = 0;

	//Dimensions
	const glm::vec2 relativeTopLeft;	//Relative to MenuCanvas
	const glm::vec2 relativeBottomRight;
	float left;
	float top;
	float right;
	float bottom;

	//Color
	glm::vec3 onColor = glm::vec3(1.0f, 1.0f, 0.6f);
	glm::vec3 offColor = glm::vec3(1.0f);
	glm::vec3 color = glm::vec3(1.0f);
};