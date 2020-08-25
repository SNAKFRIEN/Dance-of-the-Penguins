#include "AudioSource.h"

AudioSource::AudioSource(std::string name, AudioManager& audioManager)
{
	source = audioManager.CreateSource(name);
	
	alSourcef(source, AL_PITCH, 1.0f);
	alSourcef(source, AL_GAIN, 1.0f);
	alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
	alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
}

AudioSource::~AudioSource()
{
	if (source != 0)
	{
		alDeleteSources(1, &source);
	}
}

AudioSource::AudioSource(AudioSource&& rhs)
	:
	source(rhs.source)
{
	rhs.source = 0;
}

void AudioSource::Play()
{
	alSourcePlay(source);
}

void AudioSource::SetPos(glm::vec3 pos)
{
	alSource3f(source, AL_POSITION, pos.x, pos.y, pos.z);
}

void AudioSource::SetVel(glm::vec3 vel)
{
	alSource3f(source, AL_VELOCITY, vel.x, vel.y, vel.z);
}

void AudioSource::SetLooping(bool looping)
{
	alSourcei(source, AL_LOOPING, looping);
}
