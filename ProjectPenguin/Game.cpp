#include "Game.h"

#include "Window.h"
#include <iomanip>

//REMOVE
#include <glm/gtc/matrix_transform.hpp>

Game::Game(Window& window)
	:
	window(window),
	player(glm::vec3(0.0f, 0.0f, 0.0f)),
	input(window)
{
	window.SetMainCamera(&camera);
	camera.SetPos(glm::vec3(0.0f, 10.0f, 1.0f));
	
	//Seed randomness for penguin spawns, REPLACE if there's a better way
	srand(std::random_device()());

	for (int i = 0; i < 100; i++)
	{
		penguins.emplace_back(glm::vec3(2.0f, 0.0f, 0.0f));
	}
}

void Game::Update()
{
	//REMOVE: output fps
	std::cout << "fps: " << std::fixed << std::setprecision(5) << (1.0f / ft.Mark()) << std::endl;

	//For debugging purposes, fine tune camera altitude
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
	AnimatedModel::DrawAllInstances();
	iceRink.Draw(camera);
}
