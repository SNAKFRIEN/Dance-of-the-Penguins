#pragma once

#include "UIButton.h"
#include "Window.h"

#include <unordered_map>

/*This class is a container for UI elements. Calling its Update function makes sure that buttons maintain their aspect ratio no matter the size of the window*/

class MenuCanvas
{
public:
	MenuCanvas(const Window& window, float aspectRatio);

	void AddButton(glm::vec2 topLeft, glm::vec2 bottomRight, std::string name, std::string textureName = "DefaultButton.png");

	UIButton& GetButton(std::string name);

	//Recalculate dimensions based on window aspect ratio
	void Update();

	//Render all buttons and then clear the canvas
	void Draw();
private:
	const Window& window;

	//UI elements
	std::unordered_map<std::string, UIButton> buttons;

	//Menu dimensions
	const float aspectRatio;
	float width;
	float height;
};