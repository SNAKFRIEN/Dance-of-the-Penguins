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
#include "AudioManager.h"
#include "FishingPenguin.h"
#include "Light.h"
#include "ScreenQuad.h"
#include "ScreenEffect.h"
#include "PenguinDresser.h"
#include "PenguinStack.h"

class Window;

class Game
{
private:
	enum class State
	{
		MainMenu,
		Playing,
		Paused,
		GameOverCam,
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

	void SetUpBakedShadows();

	void StartPlaying();

	void UpdatePlaying(float frameTime);
	void UpdatePauseMenu();
	void UpdateMainMenu();
	void UpdateGameOverCam(float frameTime);
	void UpdateGameOver();

	void EndPlaying();

	void DrawShadows();
	void DrawPlaying();
	void DrawGamePlayUI();
	void DrawPauseMenu();
	void DrawMainMenu();
	void DrawGameOverMenu();
private:
	Window& window;
	Camera camera;
	Input input;

	std::mt19937 rng;

	IceSkater player;
	std::vector<Penguin> penguins;
	std::unique_ptr<FishingPenguin> fishingPenguin;
	std::unique_ptr<PenguinStack> penguinStack;

	int score = 0;
	static constexpr float scoreInterval = 1.0f;
	float scoreTimer = 0.0f;
	int highScore = 0;

	IceRink iceRink;

	Spawner penguinSpawner;
	PenguinDresser penguinDresser;
	//Regular penguins
	static constexpr float penguinSpawnInterval = 4.0f;
	float penguinSpawnTimer = 0.0f;
	static constexpr int maxPenguins = 100;
	//Fishing penguin
	static constexpr float fishingPenguinSpawnTime = 50.0f;
	bool fishingPenguinSpawned = false;
	std::uniform_real_distribution<float> fishingPenguinRotationRange;
	//Stacked penguins
	//REPLACE these values
	static constexpr float stackedPenguinSpawnTime = 100.0f;
	std::uniform_real_distribution<float> randomStackSpawnInterval;
	float penguinStackSpawnTimer = 0.01f;

	FrameTimer ft;
	float accumulator = 0.01f;
	static constexpr float deltaTime = 0.005f;	//Update physics 200 times per second
	float totalPlayTime = 0.0f;

	State state = State::MainMenu;
	bool quit = false;

	UICanvas mainMenu;
	UICanvas pauseMenu;
	UICanvas gameOverMenu;

	UICanvas gameplayUI;

	SaveFile saveFile;

	AudioManager audioManager;

	Light light;

	ScreenQuad screenQuad;
	ScreenEffect screenEffect;

	const int maxGameOverFlashes = 3;
	int nGameOverFlashes = 0;
};