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
	mainMenu(window, 1.0f),
	pauseMenu(window, 1.0f),
	gameOverMenu(window, 1.0f)
{
	window.SetMainCamera(&camera);
	camera.SetPos(glm::vec3(0.0f, 10.0f, 1.0f));
	
	//Seed randomness for penguin spawns, REPLACE if there's a better way
	srand(std::random_device()());

	ft.Mark();

	SetUpMainMenu();
	SetUpPauseMenu();
	SetUpGameOverMenu();
}

void Game::Update()
{
	switch (state)
	{
	case State::Playing:
		UpdatePlaying();
		break;
	case State::Paused:
		UpdatePauseMenu();
		break;
	case State::MainMenu:
		UpdateMainMenu();
		break;
	case State::GameOver:
		UpdateGameOver();
		break;
	}
}

void Game::Draw()
{
	switch (state)
	{
	case State::Playing:
		DrawPlaying();
		break;
	case State::Paused:
		DrawPlaying();	//Still show paused gameplay in the background
		DrawPauseMenu();
		break;
	case State::MainMenu:
		DrawMainMenu();
		break;
	case State::GameOver:
		DrawPlaying();	//Still show paused gameplay in the background
		DrawGameOverMenu();
	}
}

bool Game::ReadyToQuit() const
{
	return quit;
}

void Game::SetUpMainMenu()
{
	mainMenu.AddButton(glm::vec2(-0.4f, 0.4f), glm::vec2(0.4f, 0.1f), "Start", "Start.png");
	mainMenu.AddButton(glm::vec2(-0.4f, -0.1f), glm::vec2(0.4f, -0.4f), "Quit", "Quit.png");
}

void Game::SetUpPauseMenu()
{
	pauseMenu.AddButton(glm::vec2(-0.4f, 0.4f), glm::vec2(0.4f, 0.1f), "Resume", "Resume.png");
	pauseMenu.AddButton(glm::vec2(-0.4f, -0.1f), glm::vec2(0.4f, -0.4f), "Quit", "Quit.png");
}

void Game::SetUpGameOverMenu()
{
	gameOverMenu.AddButton(glm::vec2(-0.4f, 0.4f), glm::vec2(0.4f, 0.1f), "Retry", "Retry.png");
	gameOverMenu.AddButton(glm::vec2(-0.4f, -0.1f), glm::vec2(0.4f, -0.4f), "Quit", "Quit.png");
}

void Game::StartPlaying()
{
	player.Reset();

	penguins.reserve(100);
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			penguins.emplace_back(glm::vec3(x - 11, 0, y - 5));
		}
	}

	state = State::Playing;

	//Ensure at least one physics update takes place before rendering the first frame of gameplay
	UpdatePlaying();
}

void Game::UpdatePlaying()
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

		if (player.IsColliding(penguins, iceRink))
		{
			state = State::GameOver;
		}

		accumulator -= deltaTime;
	}

	//REMOVE output fps and player pos
	std::cout << "fps: " << std::fixed << std::setprecision(2) << (1.0f / frameTime) << std::endl;
	//std::cout << "Player x: " << std::fixed << std::setprecision(2) << player.GetPos().x << " y: " << player.GetPos().z << std::endl;

	if (input.IsShortPressed(InputAction::Pause))
	{
		state = State::Paused;
	}
}

void Game::UpdatePauseMenu()
{
	pauseMenu.Update();
	if (input.IsShortPressed(InputAction::Pause))
	{
		state = State::Playing;
	}
	if (pauseMenu.GetButton("Resume").UpdateAndCheckClick(input))
	{
		state = State::Playing;
	}
	if (pauseMenu.GetButton("Quit").UpdateAndCheckClick(input))
	{
		EndPlaying();
	}
}

void Game::UpdateMainMenu()
{
	mainMenu.Update();
	if (mainMenu.GetButton("Start").UpdateAndCheckClick(input))
	{
		StartPlaying();
	}
	if (mainMenu.GetButton("Quit").UpdateAndCheckClick(input))
	{
		quit = true;
	}
}

void Game::UpdateGameOver()
{
	gameOverMenu.Update();
	if (gameOverMenu.GetButton("Retry").UpdateAndCheckClick(input))
	{
		EndPlaying();
		StartPlaying();
	}
	if (gameOverMenu.GetButton("Quit").UpdateAndCheckClick(input))
	{
		EndPlaying();
	}
}

void Game::EndPlaying()
{
	penguins.clear();
	state = State::MainMenu;
}

void Game::DrawPlaying()
{
	player.Draw(camera);
	for (Penguin& p : penguins)
	{
		p.Draw(camera);
	}
	AnimatedModel::DrawAllInstances();
	iceRink.Draw(camera);
}

void Game::DrawPauseMenu()
{
	glEnable(GL_BLEND);
	pauseMenu.Draw();
	glDisable(GL_BLEND);
}

void Game::DrawMainMenu()
{
	glEnable(GL_BLEND);
	mainMenu.Draw();
	glDisable(GL_BLEND);
}

void Game::DrawGameOverMenu()
{
	glEnable(GL_BLEND);
	gameOverMenu.Draw();
	glDisable(GL_BLEND);
}
