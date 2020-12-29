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
#include "Collectible.h"
#include "HomingPenguin.h"
#include "Choir.h"
#include "SmokeMachine.h"
#include "Plus5EffectDispenser.h"

class Window;

class Game
{
private:
	enum class State
	{
		MainMenu,
		Tutorial,
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
	void SetUpTutorialUI();

	void SetUpBakedShadows();

	void StartPlaying();

	void UpdateTutorial(float frameTime);
	void UpdatePlaying(float frameTime);
	void UpdatePauseMenu();
	void UpdateMainMenuCam(float frameTime);
	void UpdateMainMenu(float frameTime);
	void UpdateGameOverCam(float frameTime);
	void UpdateGameOver();

	void EndPlaying();

	void DrawShadows();
	void DrawPlaying();
	void DrawGamePlayUI();
	void DrawTutorialUI();
	void DrawPauseMenu();
	void DrawMainMenu();
	void DrawGameOverMenu();

	std::vector<glm::vec3> GetCandyCanePositions() const;
private:
	Window& window;
	Camera camera;
	Input input;
	AudioManager audioManager;

	std::mt19937 rng;

	IceSkater player;
	std::vector<Penguin> penguins;
	std::unique_ptr<FishingPenguin> fishingPenguin;
	std::unique_ptr<PenguinStack> penguinStack;

	int score = 0;
	static constexpr float scoreInterval = 5000.0f;
	float scoreTimer = 0.0f;
	int highScore = 0;	//REMOVE: Why does this variable exist??
	bool newBest;

	std::vector<Collectible> collectibles;
	static constexpr float collectibleInterval = 400000.0f;
	float collectibleTimer = 0.0f;

	IceRink iceRink;
	static constexpr float audienceBlock1SwitchTime = 30.0f;
	static constexpr float audienceBlock2SwitchTime = 60.0f;
	static constexpr float audienceBlock3SwitchTime = 120.0f;

	Spawner spawner;
	PenguinDresser penguinDresser;
	//Regular penguins
	static constexpr float penguinSpawnInterval = 0.01f;
	float penguinSpawnTimer = 0.0f;
	static constexpr int maxPenguins = 100;
	//Fishing penguin
	static constexpr float fishingPenguinSpawnTime = 0.0f;
	bool fishingPenguinSpawned = false;
	std::uniform_real_distribution<float> fishingPenguinRotationRange;
	//Stacked penguins
	//REPLACE these values
	std::uniform_real_distribution<float> randomStackSpawnInterval;
	static constexpr float stackedPenguinSpawnTime = 100000.0f;	//Stacks won't spawn until this far into the game
	float penguinStackSpawnTimer = stackedPenguinSpawnTime;
	//Ice skating penguins
	static constexpr float homingPenguinSpawnTime = 4000.0f;
	static constexpr float homingPenguinSpawnInterval = 30.0f;
	float homingPenguinSpawnTimer = homingPenguinSpawnInterval;	//Needs to start at max, so the first penguin spawns right away when homingPenguinSpawnTime is reached
	static constexpr int maxHomingPenguins = 4;
	std::vector<HomingPenguin> homingPenguins;

	FrameTimer ft;
	float accumulator = 0.01f;
	static constexpr float deltaTime = 0.01f;	//Update physics 100 times per second
	float totalPlayTime = 0.0f;

	State state = State::MainMenu;
	bool quit = false;
	bool tutorialFinished = false;

	UICanvas mainMenu;
	UICanvas pauseMenu;
	UICanvas gameOverMenu;

	UICanvas gameplayUI;
	UICanvas tutorialUI;

	SaveFile saveFile;

	static constexpr glm::vec3 menuCamPos = glm::vec3(0.0f, 15.0f, 60.0f);
	static constexpr glm::vec3 menuCamRotation = glm::vec3(0);
	glm::vec3 currentCamLookat = glm::vec3(0.0f);

	Light light;

	ScreenQuad screenQuad;
	ScreenEffect screenEffect;

	const int maxGameOverFlashes = 1;	//Used to be multiple :(
	int nGameOverFlashes = 0;

	AudioSource gameOverFlashSound;
	AudioSource bonkSound;
	AudioSource iceSkatingSound0;
	AudioSource iceSkatingSound1;
	AudioSource penguinStackSound;
	AudioSource penguinStackFallSound;
	AudioSource windSound;
	float windChimeSoundCountDown = 2.0f;
	std::uniform_real_distribution<float> randomWindChimeInterval;
	AudioSource windChimeSound;
	AudioSource candyCaneSound;

	glm::vec3 defabc = glm::vec3(0);
	float abcrot = 0.0f;


	SmokeMachine smokeMachine;
	Plus5EffectDispenser plus5Dispenser;
};