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
	void Stop();

	void SetPos(glm::vec3 pos);
	void SetVel(glm::vec3 vel);
	void SetLooping(bool looping);
	void SetPitch(float pitch);
	void SetVolume(float volume);
	void SetFollowListener(bool follow);
	void SetRollOff(float value);
private:
	ALuint source = 0;
	const AudioManager& audioManager;
	bool followListener;
};