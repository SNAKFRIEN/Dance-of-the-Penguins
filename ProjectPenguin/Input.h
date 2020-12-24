#pragma once

#include <unordered_map>

#include "glm/glm.hpp"

class Window;
struct GLFWgamepadstate;

enum class InputAction
{
	//---Character controls---
	Forward = 0,
	Backward,
	Right,
	Left,
	//---Menu navigation---
	Pause,
	Enter,

	NumberOfInputActions
};

class Input
{
public:
	Input(const Window& window);
	bool IsPressed(const InputAction& action) const;
	bool IsShortPressed(const InputAction& action) const;
	bool IsPressed(int key) const;
	float GetForwardAxis() const;
	float GetRightAxis() const;
	void BindKey(InputAction action, int key);

	bool LMBShortPressed() const;	//Returns true only once after the mouse button is pressed
	//Relative to the top left of the window
	glm::vec2 GetMousePos() const;
	//Get mouse pos as percentage of width and height of the window
	//This is relative to the center of the screen to lign up with UI
	glm::vec2 GetMouseUV() const;
private:
	bool GetGamepadState(GLFWgamepadstate* state) const;
private:
	const Window& window;
	std::unordered_map<InputAction, int> buttonBindings;

	mutable bool prevLMBState = false;
	mutable std::vector<bool> prevInputActionStates;
};