#include "Game.h"

#include "Window.h"
#include <iomanip>

Game::Game(Window& window)
	:
	window(window),
	player(glm::vec3(0.0f, 0.0f, 0.0f)),
	input(window),
	mainMenu(window, 1.0f),
	pauseMenu(window, 1.0f),
	gameOverMenu(window, 1.0f),
	gameplayUI(window, 1.0f),
	fishingPenguinRotationRange(1.57079f, 4.71238f),
	rng(std::random_device()())
{
	window.SetMainCamera(&camera);
	camera.SetPos(glm::vec3(0.0f, 10.0f, 1.0f));
	
	//Seed randomness for penguin spawns, REPLACE if there's a better way
	srand(std::random_device()());

	SetUpMainMenu();
	SetUpPauseMenu();
	SetUpGameOverMenu();

	SetUpGameplayUI();

	saveFile.LoadData("SaveData.json");
	highScore = saveFile.GetHighScore();

	//Preload some models to save time later
	AnimatedModel::Preload("Goopie.gltf");
	Model::Preload("Crate.gltf");
	Model::Preload("FishingPole.gltf");
	Model::Preload("Bucket.gltf");
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
	gameOverMenu.AddNumberDisplay(glm::vec2(0.0f, 0.7f), glm::vec2(0.1f, 0.2f), Anchor::Center, "Score");
	gameOverMenu.AddNumberDisplay(glm::vec2(0.0f, 0.55f), glm::vec2(0.05f, 0.1f), Anchor::Center, "HighScore");
}

void Game::SetUpGameplayUI()
{
	gameplayUI.AddNumberDisplay(glm::vec2(0.0f, 0.9f), glm::vec2(0.03f, 0.06f), Anchor::Center, "Score");
	gameplayUI.GetNumberDisplay("Score").SetNumber(score);
}

void Game::StartPlaying()
{
	//Clear previous run
	penguins.clear();
	
	score = 0;
	scoreTimer = 0.0f;
	
	totalPlayTime = 0.0f;

	fishingPenguin.reset();
	fishingPenguinSpawned = false;
	iceRink.Reset();

	//Set things up for new run
	player.Reset();

	penguins.reserve(maxPenguins);
	penguinSpawnTimer = 0.0f;

	state = State::Playing;

	gameplayUI.GetNumberDisplay("Score").SetNumber(score);

	ft.Mark();

	//Ensure at least one physics update takes place before rendering the first frame of gameplay
	UpdatePlaying();
}

void Game::UpdatePlaying()
{
	const float frameTime = ft.Mark();
	totalPlayTime += frameTime;

	//Spawn new penguins
	if (penguins.size() < maxPenguins)
	{
		penguinSpawnTimer += frameTime;
		if (penguinSpawnTimer > penguinSpawnInterval)
		{
			penguins.emplace_back(penguinSpawner.FindOffScreenSpawnPoint(camera.GetPos(), player.GetPos(), camera.GetFOVRadians(), 1.0f), audioManager);
			penguinSpawnTimer = 0.0f;
		}
	}
	//Spawn fishingPenguin
	if (!fishingPenguinSpawned && totalPlayTime >= fishingPenguinSpawnTime)
	{
		auto spawn = penguinSpawner.FindDistancedSpawnPoint(player.GetPos(),
			10.0f,
			iceRink.GetRight() - iceRink.GetCornerRadius(),
			iceRink.GetTop() - iceRink.GetCornerRadius());
		float rotation = fishingPenguinRotationRange(rng);
		iceRink.SetIcePos(spawn);
		fishingPenguin = std::make_unique<FishingPenguin>(spawn, rotation, audioManager);
		fishingPenguinSpawned = true;
	}

	//Update entities
	accumulator += frameTime;
	accumulator = std::min(accumulator, 0.02f);
	while (accumulator > deltaTime)
	{
		player.Update(deltaTime, input);
		for (Penguin& penguin : penguins)
		{
			penguin.Update(deltaTime);
		}
		camera.Follow(player.GetPos());

		accumulator -= deltaTime;
	}

	//Check collisions
	for (int i = 0; i < penguins.size(); i++)
	{
		penguins[i].Collide(i, penguins, fishingPenguin, iceRink);
	}
	
	if (player.IsColliding(penguins, fishingPenguin, iceRink))
	{
		state = State::GameOver;
	}


	//Update audio listener
	audioManager.SetListenerPosition(camera.GetPos());
	audioManager.SetListenerOrientation(glm::normalize(player.GetPos() - camera.GetPos()));

	//Update animations
	player.UpdateAnimation(frameTime);
	for (Penguin& penguin : penguins)
	{
		penguin.UpdateAnimation(frameTime);
	}
	if (fishingPenguinSpawned)
	{
		fishingPenguin->UpdateAnimation(frameTime);
	}

	if (state == State::GameOver)
	{
		EndPlaying();
	}

	//Update score
	scoreTimer += frameTime;
	while (scoreTimer > scoreInterval)
	{
		score++;
		gameplayUI.GetNumberDisplay("Score").SetNumber(score);
		scoreTimer -= 1.0f;
	}
	gameplayUI.Update();

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
		ft.Mark();
		state = State::Playing;
	}
	if (pauseMenu.GetButton("Quit").UpdateAndCheckClick(input))
	{
		EndPlaying();
		state = State::MainMenu;
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
		StartPlaying();
	}
	if (gameOverMenu.GetButton("Quit").UpdateAndCheckClick(input))
	{
		state = State::MainMenu;
	}
}

void Game::EndPlaying()
{
	if (score > highScore)
	{
		highScore = score;
	}

	gameOverMenu.GetNumberDisplay("Score").SetNumber(score);
	gameOverMenu.GetNumberDisplay("HighScore").SetNumber(highScore);

	saveFile.SetHighScore(highScore);
	saveFile.SaveData("SaveData.json");

	score = 0;
	scoreTimer = 0.0f;
}

void Game::DrawPlaying()
{
	player.Draw(camera);
	for (Penguin& p : penguins)
	{
		p.Draw(camera);
	}
	if (fishingPenguinSpawned)
	{
		fishingPenguin->Draw(camera);
	}
	iceRink.Draw(camera, input);
	
	AnimatedModel::DrawAllInstances();
	Model::DrawAllInstances();

	glEnable(GL_BLEND);
	gameplayUI.Draw();
	glDisable(GL_BLEND);
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
