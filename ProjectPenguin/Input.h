#pragma once

#include <unordered_map>

class Window;

enum class InputAction
{
	Forward,
	Backward,
	Right,
	Left
};

class Input
{
public:
	Input(const Window& window);
	bool IsPressed(const InputAction& action) const;
	bool IsPressed(int key) const;
	void BindKey(InputAction action, int key);
private:
	const Window& window;
	std::unordered_map<InputAction, int> buttonBindings;
};