#pragma once

#include "UIButton.h"
#include "Window.h"

#include <unordered_map>

/*REPLACE/REMOVE this comment
The intention of the MenuCanvas is to create a single menu canvas and add buttons as they are required by the game.

The canvas needs to be repopulated every frame, and automatically clears all the buttons when it's rendered.

It's also very important that SetAspectRatio is called every frame to ensure that the menu has the correct size regardless of
the size of the actual window.

The buttons on the canvas can be accessed and interacted with by name.
*/

class MenuCanvas
{
public:
	MenuCanvas(const Window& window, float aspectRatio);

	//REPLACE: Add functionality to select a texture for the button
	//REPLACE: Make sure textures are stored so they aren't re-imported every single frame
	void AddButton(glm::vec2 topLeft, glm::vec2 bottomRight, std::string name);

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