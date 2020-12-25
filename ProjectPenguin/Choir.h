#pragma once

#include "Model.h"
#include "AnimatedModel.h"
#include "MIDIPlayer.h"

class Choir
{
private:
	enum class State
	{
		Singing,
		CountDown
	};
public:
	Choir(AudioManager& audioManager);
	void Update(float deltaTime, float totalTime);
	void Draw(Camera camera);
private:
	glm::mat4 baseTransform;
	std::vector<glm::mat4> penguinRotationAndTranslationMatrices;
	std::vector<glm::mat4> penguinTransforms;
	glm::mat4 conductorTransform;

	Model stand;
	std::vector<Model> penguins;
	AnimatedModel conductor;
	Model leadSinger;
	
	std::vector<MIDIPlayer> songs;
	int currentSongIndex;

	State state = State::CountDown;
	float timer = 10.0f;
};