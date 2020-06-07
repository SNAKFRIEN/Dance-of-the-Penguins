#include "Input.h"

#include "Window.h"

Input::Input(const Window& window)
	:
	window(window)
{
	//Load default button bindings
	//REPLACE: somehow load button bindings from file?
	BindKey(InputAction::Forward, GLFW_KEY_W);
	BindKey(InputAction::Backward, GLFW_KEY_S);
	BindKey(InputAction::Left, GLFW_KEY_A);
	BindKey(InputAction::Right, GLFW_KEY_D);
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
		assert(false);
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
