#pragma once

#include <string>

#include "AL/al.h"
#include "AL/alc.h"
#include "glm/glm.hpp"

#include <unordered_map>

class AudioManager
{
public:
	AudioManager();
	~AudioManager();
	AudioManager(const AudioManager& rhs) = delete;
	AudioManager operator=(const AudioManager& rhs) = delete;
	AudioManager(AudioManager&& rhs) = delete;
	AudioManager operator=(AudioManager&& rhs) = delete;

	ALuint CreateSource(std::string name);

	void SetListenerPosition(glm::vec3 pos);
	void SetListenerVelocity(glm::vec3 vel);
	void SetListenerOrientation(glm::vec3 lookAtDir);
private:
	ALenum GetFormat(short bitsPerSample, short channels) const;
private:
	ALCdevice* device;
	ALCcontext* context;

	std::unordered_map<std::string, ALuint> buffers;
};