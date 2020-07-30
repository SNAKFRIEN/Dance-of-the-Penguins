#include "Game.h"

#include "Window.h"
#include <iomanip>

//REMOVE
#include <glm/gtc/matrix_transform.hpp>

Game::Game(Window& window)
	:
	window(window),
	player(glm::vec3(0.0f, 0.0f, 0.0f)),
	input(window),
	pauseMenu(window, 1.0f)
{
	window.SetMainCamera(&camera);
	camera.SetPos(glm::vec3(0.0f, 10.0f, 1.0f));
	
	//Seed randomness for penguin spawns, REPLACE if there's a better way
	srand(std::random_device()());

	//penguins.reserve(100);
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			penguins.emplace_back(glm::vec3(x - 5, 0, y - 5));
		}
	}

	ft.Mark();

	SetUpPauseMenu();
}

void Game::Update()
{
	if (state == State::Playing)
	{
		const float frameTime = ft.Mark();
		accumulator += frameTime;
		accumulator = std::min(accumulator, 0.02f);
		while (accumulator > deltaTime)
		{
			player.Update(deltaTime, input);
			for (Penguin& penguin : penguins)
			{
				penguin.Update(deltaTime);
			}
			for (int i = 0; i < penguins.size(); i++)
			{
				penguins[i].Collide(i, penguins);
			}
			camera.Follow(player.GetPos());

			accumulator -= deltaTime;
		}

		//REMOVE output fps and player pos
		std::cout << "fps: " << std::fixed << std::setprecision(2) << (1.0f / frameTime) << std::endl;
		//std::cout << "Player x: " << std::fixed << std::setprecision(2) << player.GetPos().x << " y: " << player.GetPos().z << std::endl;

		if(input.IsPressed(InputAction::Pause))
		{
			state = State::Paused;
		}
	}
	else //State = Paused
	{
		pauseMenu.Update();
		if (input.IsPressed(InputAction::Pause))
		{
			state = State::Playing;
		}
		if (pauseMenu.GetButton("Resume").UpdateAndCheckClick(input))
		{
			state = State::Playing;
		}
		if (pauseMenu.GetButton("Quit").UpdateAndCheckClick(input))
		{
			quit = true;
		}
	}
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
	if (state == State::Paused)
	{
		pauseMenu.Draw();
	}
}

bool Game::ReadyToQuit() const
{
	return quit;
}

void Game::SetUpPauseMenu()
{
	pauseMenu.AddButton(glm::vec2(-0.4f, 0.4f), glm::vec2(0.4f, 0.1f), "Resume");
	pauseMenu.AddButton(glm::vec2(-0.4f, -0.1f), glm::vec2(0.4f, -0.4f), "Quit");
}