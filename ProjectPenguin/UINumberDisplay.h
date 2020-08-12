#pragma once

#include "Shader.h"

class Input;

enum class Anchor
{
	Left,
	Center,
	Right
};

class UINumberDisplay
{
public:
	UINumberDisplay(glm::vec2 pos, glm::vec2 letterScale, Anchor anchor, glm::vec2 relativePos, glm::vec2 relativeLetterScale, std::string textureName = "Numbers.png");
	~UINumberDisplay();
	UINumberDisplay(const UINumberDisplay&) = delete;
	UINumberDisplay operator=(const UINumberDisplay&) = delete;
	UINumberDisplay(UINumberDisplay&& rhs) noexcept;
	UINumberDisplay operator=(UINumberDisplay&& rhs) = delete;

	void UpdateSize(glm::vec2 newPos, glm::vec2 newScale);
	void SetNumber(unsigned int value);
	void Draw();

	glm::vec2 GetRelativePos() const;
	glm::vec2 GetRelativeScale() const;
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
	const glm::vec2 relativePos;	//Relative to MenuCanvas
	const glm::vec2 relativeLetterScale;
	glm::vec2 pos;
	glm::vec2 letterScale;
	const Anchor anchor;

	//Content
	std::vector<int> displayValue;
};