#include "Input.h"

#include "Window.h"

Input::Input(const Window& window)
	:
	window(window)
{
	prevInputActionStates.resize((unsigned int)InputAction::NumberOfInputActions, false);

	//Load default button bindings
	//REPLACE: somehow load button bindings from file?
	BindKey(InputAction::Forward, GLFW_KEY_W);
	BindKey(InputAction::Backward, GLFW_KEY_S);
	BindKey(InputAction::Left, GLFW_KEY_A);
	BindKey(InputAction::Right, GLFW_KEY_D);
	BindKey(InputAction::Pause, GLFW_KEY_ESCAPE);
}

bool Input::IsPressed(const InputAction& action) const
{
	auto buttonIterator = buttonBindings.find(action);
	if (buttonIterator != buttonBindings.end())
	{
		return window.KeyIsPressed(buttonIterator->second);
	}
	else
	{
		assert(false);	//InputAction not bound??
		return false;
	}
}

bool Input::IsShortPressed(const InputAction& action) const
{
	auto buttonIterator = buttonBindings.find(action);
	if (buttonIterator != buttonBindings.end())
	{
		bool result = !prevInputActionStates[(unsigned int)action] && window.KeyIsPressed(buttonIterator->second);
		prevInputActionStates[(unsigned int)action] = window.KeyIsPressed(buttonIterator->second);
		return result;
	}
	else
	{
		assert(false);	//InputAction not bound??
		return false;
	}
}

bool Input::IsPressed(int key) const
{
	return window.KeyIsPressed(key);
}

void Input::BindKey(InputAction action, int key)
{
	//REPLACE: warn the user when binding a key that's already being used elsewhere?
	buttonBindings[action] = key;
}

bool Input::LMBShortPressed() const
{
	bool result = !prevLMBState && window.MouseButtonIsPressed(GLFW_MOUSE_BUTTON_LEFT);
	prevLMBState = window.MouseButtonIsPressed(GLFW_MOUSE_BUTTON_LEFT);
	return result;
}

glm::vec2 Input::GetMousePos() const
{
	return window.GetCursorPos();
}

glm::vec2 Input::GetMouseUV() const
{
	glm::vec2 result = window.GetCursorPos() / window.GetDimensions();
	result -= glm::vec2(0.5f);
	result.y *= -1.0f;
	result *= 2.0f;
	return result;
}
