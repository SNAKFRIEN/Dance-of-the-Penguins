#pragma once

#include "Camera.h"
#include "Input.h"
#include "IceSkater.h"
#include "Penguin.h"

class Window;

class Game
{
public:
	Game(Window& window);
	void Update();
	void Draw();
private:
	Window& window;
	Camera camera;
	Input input;

	IceSkater player;
	std::vector<Penguin> penguins;

	Model ice;
	glm::mat4 iceTransform = glm::mat4(1.0f);
};