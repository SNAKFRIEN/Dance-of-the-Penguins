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

	for (int i = 0; i < 500; i++)
	{
		penguins.emplace_back(glm::vec3(2.0f, 0.0f, 0.0f));
	}

	ft.Mark();
}

void Game::Update()
{
	const float frameTime = ft.Mark();
	accumulator += frameTime;
	accumulator = std::min(accumulator, 0.1f);
	while (accumulator > deltaTime)
	{
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

		player.Update(deltaTime, input);
		for (int i = 0; i < penguins.size(); i++)
		{
			penguins[i].Update(deltaTime);
			penguins[i].Collide(i, penguins);
		}
		camera.Follow(player.GetPos());

		accumulator -= deltaTime;
	}
	//REMOVE output fps
	std::cout << "fps: " << std::fixed << std::setprecision(5) << (1.0f / frameTime) << std::endl;
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
