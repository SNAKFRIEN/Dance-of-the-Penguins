#pragma once

#include "Camera.h"
#include "Input.h"
#include "IceSkater.h"
#include "Penguin.h"
#include "IceRink.h"
#include "FrameTimer.h"
#include "UserInterface.h"

class Window;

class Game
{
private:
	enum class State
	{
		MainMenu,
		Playing,
		Paused
	};
public:
	Game(Window& window);

	void Update();
	void Draw();
	
	bool ReadyToQuit() const;
private:
	void SetUpMainMenu();
	void SetUpPauseMenu();

	void StartPlaying();

	void UpdatePlaying();
	void UpdatePauseMenu();
	void UpdateMainMenu();

	void EndPlaying();

	void DrawPlaying();
	void DrawPauseMenu();
	void DrawMainMenu();
private:
	Window& window;
	Camera camera;
	Input input;

	IceSkater player;
	std::vector<Penguin> penguins;

	IceRink iceRink;

	FrameTimer ft;
	float accumulator = 0.01f;
	static constexpr float deltaTime = 0.005f;	//Update physics 200 times per second

	State state = State::MainMenu;
	bool quit = false;

	MenuCanvas mainMenu;
	MenuCanvas pauseMenu;
};