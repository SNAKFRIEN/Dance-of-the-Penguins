#include "Game.h"

#include "Window.h"

//REMOVE
#include <glm/gtc/matrix_transform.hpp>

Game::Game(Window& window)
	:
	window(window),
	player(glm::vec3(0.0f, 0.0f, 0.0f)),
	input(window),
	ice("Ice.gltf", iceTransform)
{
	window.SetMainCamera(&camera);
	camera.SetPos(glm::vec3(0.0f, 10.0f, 1.0f));

	iceTransform = glm::translate(iceTransform, glm::vec3(0.0f, -0.7f, 0.0f));
	iceTransform = glm::scale(iceTransform, glm::vec3(10.0f, 1.0f, 10.0f));
	
	//Seed randomness for penguin spawns, REPLACE if there's a better way
	srand(std::random_device()());


	penguins.reserve(1);
	for (int i = 0; i < 1; i++)
	{
		penguins.emplace_back(glm::vec3(2.0f, 0.0f, 0.0f));
	}
}

void Game::Update()
{
	//For debuggin purposes, fine tune camera altitude
	if (window.KeyIsPressed(GLFW_KEY_X))
	{
		camera.SetPos(camera.GetPos() + glm::vec3(0.0f, 0.01f, 0.0f));
	}
	if (window.KeyIsPressed(GLFW_KEY_Z))
	{
		camera.SetPos(camera.GetPos() + glm::vec3(0.0f, -0.01f, 0.0f));
	}
	if (window.KeyIsPressed(GLFW_KEY_C))
	{
		std::cout << camera.GetPos().y << std::endl;
	}

	player.Update(0.0f, input);
	for (Penguin& p : penguins)
	{
		p.Update(0.01f);
	}
	camera.Follow(player.GetPos());
}

void Game::Draw()
{
	player.Draw(camera);
	for (Penguin& p : penguins)
	{
		p.Draw(camera);
	}
	ice.Draw(camera);
}
