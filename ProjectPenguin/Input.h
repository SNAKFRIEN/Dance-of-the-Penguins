#pragma once

#include <unordered_map>
#include "glm/glm.hpp"

class Window;

enum class InputAction
{
	//---Character controls---
	Forward,
	Backward,
	Right,
	Left,
	//---Menu navigation---
	Pause
};

class Input
{
public:
	Input(const Window& window);
	bool IsPressed(const InputAction& action) const;
	bool IsPressed(int key) const;
	void BindKey(InputAction action, int key);

	bool LMBPressed() const;
	//Relative to the top left of the window
	glm::vec2 GetMousePos() const;
	//Get mouse pos as percentage of width and height of the window
	//This is relative to the center of the screen to lign up with UI
	glm::vec2 GetMouseUV() const;
private:
	const Window& window;
	std::unordered_map<InputAction, int> buttonBindings;
};