#include "AudioManager.h"

#include <exception>

#include "WAVLoader.h"

AudioManager::AudioManager()
{
	device = alcOpenDevice(nullptr);
	if (!device)
	{
		throw std::exception("Unable to create ALCdevice");
	}
	context = alcCreateContext(device, nullptr);
	alcMakeContextCurrent(context);
	if (!context)
	{
		throw std::exception("Unable to create ALCcontext");
	}
}

AudioManager::~AudioManager()
{
	//REPLACE: Make sure to remove audio sources created via audio manager
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

ALuint AudioManager::CreateSource(std::string name)
{
	ALuint source;
	alGenSources(1, &source);

	//Create buffer if it does not exist
	if (buffers.count(name) == 0)
	{
		//Retrieve data from file
		std::string path = "Audio/";
		path.append(name);
		WAVLoader loader;
		WAVData data;
		data = loader.LoadWAV(path);
		ALenum format = GetFormat(data.bitsPerSample, data.channels);

		//Hand data over to OpenAL
		ALuint newBuffer;
		alGenBuffers(1, &newBuffer);
		buffers[name] = newBuffer;
		alBufferData(newBuffer, format, data.data.data(), data.dataSize, data.sampleRate);
	}

	//Bind buffer to source
	alSourcei(source, AL_BUFFER, buffers[name]);
	
	return source;
}

void AudioManager::SetListenerPosition(glm::vec3 pos)
{
	alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
}

void AudioManager::SetListenerVelocity(glm::vec3 vel)
{
	alListener3f(AL_VELOCITY, vel.x, vel.y, vel.z);
}

void AudioManager::SetListenerOrientation(glm::vec3 lookAtDir)
{
	ALfloat listenerOrientation[] = { lookAtDir.x, lookAtDir.y, lookAtDir.z,
		0.0f, 1.0f, 0.0f };
}

glm::vec3 AudioManager::GetListenerPosition() const
{
	float x, y, z;
    alGetListener3f(AL_POSITION, &x, &y, &z);
	return glm::vec3(x, y, z);
}

ALenum AudioManager::GetFormat(short bitsPerSample, short channels) const
{
	if (bitsPerSample == 8)
	{
		if (channels == 1)
		{
			return AL_FORMAT_MONO8;
		}
		else if (channels == 2)
		{
			return AL_FORMAT_STEREO8;
		}
	}
	else if (bitsPerSample == 16)
	{
		if (channels == 1)
		{
			return AL_FORMAT_MONO16;
		}
		else if (channels == 2)
		{
			return AL_FORMAT_STEREO16;
		}
	}
	return 0;
}
