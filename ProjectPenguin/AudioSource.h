#pragma once

#include "AudioManager.h"
#include <vector>

class AudioSource
{
public:
	AudioSource(std::string name, AudioManager& audioManager);
	~AudioSource();
	AudioSource(const AudioSource& rhs) = delete;
	AudioSource operator=(const AudioSource& rhs) = delete;
	AudioSource(AudioSource&& rhs);
	AudioSource operator=(AudioSource&& rhs) = delete;

	void Play();

	void SetPos(glm::vec3 pos);
	void SetVel(glm::vec3 vel);
	void SetLooping(bool looping);
private:
	ALuint source = 0;
};