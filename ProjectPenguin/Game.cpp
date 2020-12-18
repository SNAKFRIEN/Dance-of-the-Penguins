#include "Game.h"

#include "Window.h"
//REMOVE: iomanip likely isn't necessary for final release
#include <iomanip>

#include "glm/gtc/random.hpp"

#include "GlGetError.h"

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
	rng(std::random_device()()),
	light(glm::vec3(0.0f, 10.0f, 0.0f), saveFile.GetShadowRes()),
	screenQuad(window, saveFile),
	penguinDresser(rng),
	randomStackSpawnInterval(10.0f, 30.0f)	//REPLACE these values
{
	window.SetMainCamera(&camera);
	window.SetScreenQuad(&screenQuad);
	camera.SetPos(glm::vec3(0.0f, 10.0f, 1.0f));
	
	//Seed randomness for penguin spawns, REPLACE if there's a better way
	srand(std::random_device()());

	//Setup UI
	SetUpMainMenu();
	SetUpPauseMenu();
	SetUpGameOverMenu();

	SetUpGameplayUI();

	//Bake shadows for static objects
	SetUpBakedShadows();

	//Load save data
	saveFile.LoadData("SaveData.json");
	highScore = saveFile.GetHighScore();

	//Preload some models to save time later
	AnimatedModel::Preload("Goopie.gltf");
	Model::Preload("Crate.gltf");
	Model::Preload("FishingPole.gltf");
	Model::Preload("Bucket.gltf");
	Model::Preload("Bouquet.gltf");
}

void Game::Update()
{
	const float frameTime = ft.Mark();
	switch (state)
	{
	case State::Playing:
		UpdatePlaying(frameTime);
		break;
	case State::Paused:
		UpdatePauseMenu();
		break;
	case State::MainMenu:
		UpdateMainMenu();
		break;
	case State::GameOverCam:
		UpdateGameOverCam(frameTime);
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
		DrawGamePlayUI();
		break;
	case State::Paused:
		DrawPlaying();	//Still show paused gameplay in the background
		DrawPauseMenu();
		break;
	case State::MainMenu:
		DrawMainMenu();
		break;
	case State::GameOverCam:
		DrawPlaying();
		break;
	case State::GameOver:
		DrawPlaying();	//Still show paused gameplay in the background
		DrawGameOverMenu();
		break;
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
	gameOverMenu.AddButton(glm::vec2(-0.8, 0.9), glm::vec2(0.3, -0.4), "Background", "ScoreScreen.png");
	gameOverMenu.AddButton(glm::vec2(0.0f, -0.6f), glm::vec2(0.8f, -0.9f), "Retry", "Retry.png");
	gameOverMenu.AddButton(glm::vec2(-0.8f, -0.6f), glm::vec2(0.0f, -0.9f), "Quit", "Quit.png");
	gameOverMenu.AddNumberDisplay(glm::vec2(0.0f, 0.7f), glm::vec2(0.1f, 0.2f), Anchor::Center, "Score");
	gameOverMenu.AddNumberDisplay(glm::vec2(0.0f, 0.55f), glm::vec2(0.05f, 0.1f), Anchor::Center, "HighScore");
}

void Game::SetUpGameplayUI()
{
	gameplayUI.AddNumberDisplay(glm::vec2(0.0f, 0.9f), glm::vec2(0.03f, 0.06f), Anchor::Center, "Score");
	gameplayUI.GetNumberDisplay("Score").SetNumber(score);
}

void Game::SetUpBakedShadows()
{
	//Draw all objects that can be baked
	iceRink.DrawStatic(camera, input);

	light.UseBakeTexture();

	glCullFace(GL_FRONT);

	//Prepare shadow FBO
	glViewport(0, 0, light.GetShadowResolutionX(), light.GetShadowResolutionY());
	glBindFramebuffer(GL_FRAMEBUFFER, light.GetFBO());
	glClear(GL_DEPTH_BUFFER_BIT);
	GL_ERROR_CHECK();
	//Bind shader and draw shadows
	light.UseNonAnimationShader();
	Model::DrawShadows(light);
	//Revert to default FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window.GetWidth(), window.GetHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glCullFace(GL_BACK);

	light.UseNonBakeTexture();
}

void Game::StartPlaying()
{
	//Clear previous run
	penguins.clear();
	collectibles.clear();
	homingPenguins.clear();
	penguinStack.reset();

	score = 0;
	scoreTimer = 0.0f;
	
	totalPlayTime = 0.0f;

	fishingPenguin.reset();
	fishingPenguinSpawned = false;
	iceRink.Reset();

	//Reset spawn timers
	collectibleTimer = 0.0f;
	penguinSpawnTimer = 0.0f;
	penguinStackSpawnTimer = penguinStackInitialSpawnTimerValue;
	homingPenguinSpawnTimer = homingPenguinSpawnInterval;

	//Set things up for new run
	player.Reset();

	penguins.reserve(maxPenguins);
	penguinSpawnTimer = 0.0f;

	state = State::Playing;

	gameplayUI.GetNumberDisplay("Score").SetNumber(score);

	ft.Mark();

	window.HideMouse();

	//Ensure at least one physics update takes place before rendering the first frame of gameplay
	UpdatePlaying(0.01f);
}

void Game::UpdatePlaying(float frameTime)
{
	frameTime = std::min(frameTime, 0.1f);

	totalPlayTime += frameTime;

	//Spawn new penguins
	if (penguins.size() < maxPenguins)
	{
		penguinSpawnTimer += frameTime;
		if (penguinSpawnTimer > penguinSpawnInterval)
		{
			penguins.emplace_back(spawner.FindOffScreenSpawnPoint(camera.GetPos(), player.GetPos(), camera.GetFOVRadians(), 1.0f), audioManager);
			auto outfit = penguinDresser.GeneratePenguinOutfit();
			for (auto& accessory : outfit)
			{
				penguins[penguins.size() - 1].AddAccessory(accessory.name, accessory.bone, accessory.vertShader, accessory.fragShader);
			}
			penguinSpawnTimer -= penguinSpawnInterval;
		}
	}
	//Spawn fishingPenguin
	if (!fishingPenguinSpawned && totalPlayTime >= fishingPenguinSpawnTime)
	{
		auto spawn = spawner.FindDistancedSpawnPoint(player.GetPos(),
			10.0f,
			iceRink.GetRight() - iceRink.GetCornerRadius(),
			iceRink.GetTop() - iceRink.GetCornerRadius());
		float rotation = fishingPenguinRotationRange(rng);
		iceRink.SetIcePos(spawn);
		fishingPenguin = std::make_unique<FishingPenguin>(spawn, rotation, audioManager);
		fishingPenguinSpawned = true;
	}
	//Spawn penguin stack
	penguinStackSpawnTimer -= frameTime;
	if (penguinStackSpawnTimer <= 0.0f && totalPlayTime >= stackedPenguinSpawnTime)
	{
		penguinStackSpawnTimer += randomStackSpawnInterval(rng);
		auto stackSpawn = spawner.FindDistancedSpawnPoint(player.GetPos(),
			12.0f,
			iceRink.GetRight() - iceRink.GetCornerRadius(),
			iceRink.GetTop() - iceRink.GetCornerRadius());
		auto stackTarget = spawner.FindCloseTarget(player.GetPos(), 5.0f);
		penguinStack = std::make_unique<PenguinStack>(stackSpawn, stackTarget, rng, audioManager);
	}
	//Spawn collectibles
	collectibleTimer += frameTime;
	if (collectibleTimer > collectibleInterval && !input.IsPressed(GLFW_KEY_X))
	{
		collectibleTimer -= collectibleInterval;
		glm::vec3 spawn = spawner.FindDistancedSpawnPoint(player.GetPos(),
			5.0f,
			iceRink.GetRight() - iceRink.GetCornerRadius(),
			iceRink.GetTop() - iceRink.GetCornerRadius());
		spawn.y = 10.0f;
		collectibles.emplace_back(spawn);
	}
	//Spawn HomingPenguins
	if (totalPlayTime >= homingPenguinSpawnTime && homingPenguins.size() < maxHomingPenguins)
	{
		homingPenguinSpawnTimer += frameTime;
		if (homingPenguinSpawnTimer >= homingPenguinSpawnInterval)
		{
			homingPenguinSpawnTimer -= homingPenguinSpawnInterval;
			glm::vec3 spawn = spawner.FindDistancedSpawnPoint(player.GetPos(),
				10.0f,
				iceRink.GetRight() - iceRink.GetCornerRadius(),
				iceRink.GetTop() - iceRink.GetCornerRadius());
			homingPenguins.emplace_back(spawn);
		}
	}
	//Replace audience
	iceRink.SetAudienceBlock1(totalPlayTime > audienceBlock1SwitchTime);
	iceRink.SetAudienceBlock2(totalPlayTime > audienceBlock2SwitchTime);
	iceRink.SetAudienceBlock3(totalPlayTime > audienceBlock3SwitchTime);

	//Update entities (Fixed deltaTime)
	accumulator += frameTime;
	accumulator = std::min(accumulator, 0.02f);
	while (accumulator > deltaTime)
	{
		player.Update(deltaTime, input);
		//REMOVE: debug cameras
		if (input.IsPressed(GLFW_KEY_V))
		{
			camera.Follow(penguinStack->GetPos());
		}
		else if (input.IsPressed(GLFW_KEY_C))
		{
			camera.Follow(collectibles[0].GetPos());
		}
		else if (input.IsPressed(GLFW_KEY_B))
		{
			if (!homingPenguins.empty())
			{
				camera.Follow(homingPenguins[0].GetPos());
			}
		}
		else
		{
			camera.Follow(player.GetPos());
		}
		for (HomingPenguin& hp : homingPenguins)
		{
			hp.Update(player, collectibles, iceRink, deltaTime);
		}

		accumulator -= deltaTime;
	}
	//Update entities (Dynamic deltaTime)
	for (Penguin& penguin : penguins)
	{
		penguin.Update(frameTime);
	}
	if (penguinStack)
	{
		penguinStack->Update(frameTime, iceRink);
	}
	camera.CalculateVPMatrix();
	for (Collectible& c : collectibles)
	{
		c.Update(frameTime);
	}
	iceRink.Update(totalPlayTime);
	//Pick up collectibles (either by player or by homing penguin)
	{
		const auto newEnd = std::remove_if(collectibles.begin(), collectibles.end(),
			[&](Collectible& c)
			{
				for (HomingPenguin& hp : homingPenguins)
				{
					if (hp.GetCollider().CalculateCollision(c.GetCollider()).isColliding)
					{
						hp.GiveFlower();
						return true;
					}
				}
				if (player.GetCollider().CalculateCollision(c.GetCollider()).isColliding)
				{
					return true;
				}
				return false;
			});
		collectibles.erase(newEnd, collectibles.end());
	}


	//Check collisions
	for (int i = 0; i < penguins.size(); i++)
	{
		penguins[i].Collide(i, penguins, fishingPenguin, iceRink);
	}
	for (HomingPenguin& hp : homingPenguins)
	{
		hp.Collide(iceRink);
	}
	bool gameOver = false;
	if (player.IsColliding(penguins, fishingPenguin, penguinStack, homingPenguins, iceRink))
	{
		gameOver = true;
	}

	//Remove homingPenguins that have crashed into the iceRink
	{
		const auto newEnd = std::remove_if(homingPenguins.begin(), homingPenguins.end(),
			[](HomingPenguin& hp)
			{
				return hp.IsFinished();
			});
		homingPenguins.erase(newEnd, homingPenguins.end());
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
	if (penguinStack)
	{
		penguinStack->UpdateAnimation(frameTime);
	}
	for (HomingPenguin& hp : homingPenguins)
	{
		hp.UpdateAnimation(frameTime);
	}

	//Play game over animation
	if (gameOver)
	{
		state = State::GameOverCam;
		EndPlaying();
	}

	//Update score and gameplay UI
	scoreTimer += frameTime;
	while (scoreTimer > scoreInterval)
	{
		score++;
		gameplayUI.GetNumberDisplay("Score").SetNumber(score);
		scoreTimer -= 1.0f;
	}
	gameplayUI.Update();
	//Count number of homing penguins
	std::vector<HomingPenguin*> lockedOnPenguins;
	for (HomingPenguin& p : homingPenguins)
	{
		if (p.IsLockedOn())
		{
			lockedOnPenguins.emplace_back(&p);
		}
	}
	gameplayUI.SetUpPenguinWarnings((int)lockedOnPenguins.size());
	for (int i = 0; i < lockedOnPenguins.size(); i++)
	{
		gameplayUI.GetPenguinWarning(i).Update(lockedOnPenguins[i]->GetPos(), camera, window.GetDimensions());
	}

	//REMOVE output fps and player pos
	std::cout << "fps: " << std::fixed << std::setprecision(2) << (1.0f / frameTime) << std::endl;
	//std::cout << "Player x: " << std::fixed << std::setprecision(2) << player.GetPos().x << " y: " << player.GetPos().z << std::endl;

	if (input.IsShortPressed(InputAction::Pause))
	{
		state = State::Paused;
		window.ShowMouse();
	}
}

void Game::UpdatePauseMenu()
{
	pauseMenu.Update();
	if (input.IsShortPressed(InputAction::Pause))
	{
		state = State::Playing;
		window.HideMouse();
	}
	if (pauseMenu.GetButton("Resume").UpdateAndCheckClick(input))
	{
		ft.Mark();
		state = State::Playing;
		window.HideMouse();
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

void Game::UpdateGameOverCam(float frameTime)
{
	screenEffect.Update(frameTime);
	if (nGameOverFlashes >= maxGameOverFlashes && screenEffect.GetCurrentEffectType() != ScreenEffect::EffectType::Flash)
	{
		state = State::GameOver;
		nGameOverFlashes = 0;
	}
	else if (screenEffect.GetCurrentEffectType() != ScreenEffect::EffectType::Flash)
	{

		//Select camPos
		bool found = false;
		glm::vec2 camPosXZ;
		while (!found)
		{
			camPosXZ = glm::circularRand(7.0f);
			//Make sure it's in the rink
			camPosXZ += glm::vec2(player.GetPos().x, player.GetPos().z);
			float maxOutOfRink = 4.0f;
			if (camPosXZ.x < iceRink.GetRight() + maxOutOfRink
				&& camPosXZ.x > -iceRink.GetRight() - maxOutOfRink
				&& camPosXZ.y < iceRink.GetTop() + maxOutOfRink
				&& camPosXZ.y > -iceRink.GetTop() - maxOutOfRink)
			{
				found = true;
			}
		}
		float camPosY = glm::linearRand(3.0f, 13.0f);
		camera.LookAt(glm::vec3(camPosXZ.x, camPosY, camPosXZ.y), player.GetPos());
		camera.CalculateVPMatrix();
		//Do flash
		screenEffect.SetFlashEffect(1.3f);
		nGameOverFlashes++;
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

	window.ShowMouse();

	gameOverMenu.GetNumberDisplay("Score").SetNumber(score);
	gameOverMenu.GetNumberDisplay("HighScore").SetNumber(highScore);

	saveFile.SetHighScore(highScore);
	saveFile.SaveData("SaveData.json");

	score = 0;
	scoreTimer = 0.0f;
}

void Game::DrawShadows()
{
	glCullFace(GL_FRONT);
	//Prepare shadow FBO
	glViewport(0, 0, light.GetShadowResolutionX(), light.GetShadowResolutionY());
	glBindFramebuffer(GL_FRAMEBUFFER, light.GetFBO());
	glClear(GL_DEPTH_BUFFER_BIT);
	GL_ERROR_CHECK();
	//Bind shader and draw shadows
	light.UseAnimationShader();
	AnimatedModel::DrawShadows(light);
	light.UseNonAnimationShader();
	Model::DrawShadows(light);
	//Revert to default FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, (GLsizei)window.GetDimensions().x, (GLsizei)window.GetDimensions().y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_BACK);
}

void Game::DrawPlaying()
{
	//Draw all items that cast shadows
	player.Draw(camera);
	for (Penguin& p : penguins)
	{
		p.Draw(camera);
	}
	if (fishingPenguinSpawned)
	{
		fishingPenguin->Draw(camera);
	}
	if (penguinStack)
	{
		penguinStack->Draw(camera);
	}
	iceRink.DrawNonStatic(camera, input, GetFlowerPositions());
	for (HomingPenguin& hp : homingPenguins)
	{
		hp.Draw(camera);
	}

	//Cast shadows
	if (!input.IsPressed(GLFW_KEY_G))
	{
		DrawShadows();
	}

	//Draw all items that don't cast (dynamic) shadows
	iceRink.DrawStatic(camera, input);
	for (Collectible& c : collectibles)
	{
		c.Draw(camera);
	}
	
	if (!input.IsPressed(GLFW_KEY_H))
	{
		//Bind screenQuad
		screenQuad.StartFrame();
		GL_ERROR_CHECK();
	}

	//Draw all entities
	AnimatedModel::DrawAllInstances(light);
	Model::DrawAllInstances(light);

	if (!input.IsPressed(GLFW_KEY_H))
	{
		screenQuad.EndFrame();

		//Draw using effect
		screenEffect.UseEffect();
		auto screenTexture = screenQuad.GetTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, screenTexture);

		screenQuad.Draw();
	}
}

void Game::DrawGamePlayUI()
{
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

std::vector<glm::vec3> Game::GetFlowerPositions() const
{
	std::vector<glm::vec3> result;
	for (const Collectible& flower : collectibles)
	{
		result.emplace_back(flower.GetPos());
	}
	return std::move(result);
}
