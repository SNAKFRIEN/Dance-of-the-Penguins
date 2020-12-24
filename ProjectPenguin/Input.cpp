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
	BindKey(InputAction::Enter, GLFW_KEY_ENTER);
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

float Input::GetForwardAxis() const
{
	if (IsPressed(InputAction::Forward))
	{
		return 1.0f;
	}
	else if (IsPressed(InputAction::Backward))
	{
		return -1.0f;
	}
	else
	{
		GLFWgamepadstate state;
		if (GetGamepadState(&state))
		{
			return -state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
		}
	}
	return 0.0f;
}

float Input::GetRightAxis() const
{
	if (IsPressed(InputAction::Right))
	{
		return 1.0f;
	}
	else if (IsPressed(InputAction::Left))
	{
		return -1.0f;
	}
	else
	{
		GLFWgamepadstate state;
		if (GetGamepadState(&state))
		{
			return state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
		}
	}
	return 0.0f;
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

bool Input::GetGamepadState(GLFWgamepadstate* state) const
{
	//Check the state of the first controller that can be found (only the first 4 controllers are checked
	if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
	{
		glfwGetGamepadState(GLFW_JOYSTICK_1, state);
		return true;
	}
	else if (glfwJoystickIsGamepad(GLFW_JOYSTICK_2))
	{
		glfwGetGamepadState(GLFW_JOYSTICK_2, state);
		return true;
	}
	else if (glfwJoystickIsGamepad(GLFW_JOYSTICK_3))
	{
		glfwGetGamepadState(GLFW_JOYSTICK_3, state);
		return true;
	}
	else if (glfwJoystickIsGamepad(GLFW_JOYSTICK_4))
	{
		glfwGetGamepadState(GLFW_JOYSTICK_4, state);
		return true;
	}
	return false;
}
