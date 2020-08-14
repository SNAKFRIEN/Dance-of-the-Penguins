#pragma once

#include "Camera.h"
#include "Input.h"
#include "IceSkater.h"
#include "Penguin.h"
#include "IceRink.h"
#include "FrameTimer.h"
#include "UserInterface.h"
#include "JointAttachment.h"
#include "Spawner.h"
#include "SaveFile.h"

class Window;

class Game
{
private:
	enum class State
	{
		MainMenu,
		Playing,
		Paused,
		GameOver
	};
public:
	Game(Window& window);

	void Update();
	void Draw();
	
	bool ReadyToQuit() const;
private:
	void SetUpMainMenu();
	void SetUpPauseMenu();
	void SetUpGameOverMenu();
	void SetUpGameplayUI();

	void StartPlaying();

	void UpdatePlaying();
	void UpdatePauseMenu();
	void UpdateMainMenu();
	void UpdateGameOver();

	void EndPlaying();

	void DrawPlaying();
	void DrawPauseMenu();
	void DrawMainMenu();
	void DrawGameOverMenu();
private:
	Window& window;
	Camera camera;
	Input input;

	IceSkater player;
	std::vector<Penguin> penguins;
	std::unique_ptr<JointAttachment> test;

	int score = 0;
	static constexpr float scoreInterval = 1.0f;
	float scoreTimer = 0.0f;

	IceRink iceRink;

	Spawner penguinSpawner;
	static constexpr float penguinSpawnInterval = 7.0f;
	float penguinSpawnTimer = 0.0f;
	static constexpr int maxPenguins = 100;


	FrameTimer ft;
	float accumulator = 0.01f;
	static constexpr float deltaTime = 0.005f;	//Update physics 200 times per second

	State state = State::MainMenu;
	bool quit = false;

	UICanvas mainMenu;
	UICanvas pauseMenu;
	UICanvas gameOverMenu;

	UICanvas gameplayUI;

	SaveFile saveFile;
};