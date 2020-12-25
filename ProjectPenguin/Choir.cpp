#include "Choir.h"

#include "Camera.h"

Choir::Choir(AudioManager& audioManager)
	:
	baseTransform(1.0f),
	stand("ChoirStand.gltf", baseTransform, "SmoothShader.vert", "SmoothBright.frag"),
	leadSinger("BuffGoopie.gltf", baseTransform),
	conductor("Goopie.gltf", conductorTransform, "Idle")
{
	conductorTransform = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(-26.9678f, 0.012346f, -1.16834f)), 0.9388632192f, glm::vec3(0.0f, 1.0f, 0.0f));

	penguinRotationAndTranslationMatrices.emplace_back(glm::translate(glm::mat4(1.0f), glm::vec3(-28.7035f, 0.417051f, -1.3664f)));
	penguinRotationAndTranslationMatrices.emplace_back(glm::translate(glm::mat4(1.0f), glm::vec3(-28.2625f, 0.418506f, -2.08861f)));
	penguinRotationAndTranslationMatrices.emplace_back(glm::translate(glm::mat4(1.0f), glm::vec3(-27.8106f, 0.41618f, -2.75546f)));
	penguinRotationAndTranslationMatrices.emplace_back(glm::translate(glm::mat4(1.0f), glm::vec3(-29.7012f, 1.14316f, -2.07134f)));
	penguinRotationAndTranslationMatrices.emplace_back(glm::translate(glm::mat4(1.0f), glm::vec3(-28.8157f, 1.15682f, -3.39758f)));
	//Middle row (these wiggle in opposite direction)
	penguinRotationAndTranslationMatrices.emplace_back(glm::translate(glm::mat4(1.0f), glm::vec3(-29.0024f, 0.782063f, -2.05495f)));
	penguinRotationAndTranslationMatrices.emplace_back(glm::translate(glm::mat4(1.0f), glm::vec3(-28.5273f, 0.784321f, -2.79177f)));

	for (glm::mat4& m : penguinRotationAndTranslationMatrices)
	{
		m = glm::rotate(m, 4.144668281f, glm::vec3(0.0f, 1.0f, 0.0f));
		penguinTransforms.emplace_back(m);
	}

	penguins.reserve(penguinRotationAndTranslationMatrices.size());
	for (int i = 0; i < penguinRotationAndTranslationMatrices.size(); i++)
	{
		penguins.emplace_back("ChoirGoop.gltf", penguinTransforms[i]);
	}

	songs.emplace_back("DeckTheHalls.mid", "Quack.wav", 43.66f, audioManager);
	songs.emplace_back("JingleBells.mid", "Quack.wav", 21.83f, audioManager);

	for (MIDIPlayer& m : songs)
	{
		m.SetPosition(glm::vec3(-28.7035f, 0.417051f, -1.3664f));
	}
}

void Choir::Update(float deltaTime, float totalTime)
{
	switch (state)
	{
	case State::Singing:
		//Animations
		for (int i = 0; i < penguinTransforms.size() - 2; i++)
		{
			penguinTransforms[i] = penguinRotationAndTranslationMatrices[i] * glm::rotate(glm::mat4(1.0f), glm::clamp(sin(totalTime * 4.0f), -0.3f, 0.3f), glm::vec3(0.0f, 0.0f, 1.0f));
		}
		//Middle row wiggles opposite direction
		for (int i = (int)penguinTransforms.size() - 2; i < penguinTransforms.size(); i++)
		{
			penguinTransforms[i] = penguinRotationAndTranslationMatrices[i] * glm::rotate(glm::mat4(1.0f), glm::clamp(sin(-totalTime * 4.0f), -0.3f, 0.3f), glm::vec3(0.0f, 0.0f, 1.0f));
		}

		//Sound
		songs[currentSongIndex].Update(deltaTime);

		if (songs[currentSongIndex].IsFinished())
		{
			songs[currentSongIndex].Reset();
			currentSongIndex++;
			currentSongIndex %= songs.size();
			timer = 5.0f;
			state = State::CountDown;
			conductor.SetAnimation("Idle");
		}
		break;
	case State::CountDown:
		for (int i = 0; i < penguinTransforms.size(); i++)
		{
			penguinTransforms[i] = penguinRotationAndTranslationMatrices[i];
		}

		timer -= deltaTime;
		if (timer <= 0.0f)
		{
			state = State::Singing;
			conductor.SetAnimation("Conducting");
		}
		break;
	}

	conductor.Update(deltaTime);
}

void Choir::Draw(Camera camera)
{
	stand.AddToRenderQueue(camera);
	leadSinger.AddToRenderQueue(camera);
	conductor.AddToRenderQueue(camera);
	for (Model& p : penguins)
	{
		p.AddToRenderQueue(camera);
	}
}
