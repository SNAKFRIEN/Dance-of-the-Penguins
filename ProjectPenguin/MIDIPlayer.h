#pragma once

#include <string>

#include "AudioSource.h"

//REMOVE
class Camera;

class MIDIPlayer
{
private:
	struct Note
	{
		float timeStamp;
		float pitch;
	};
public:
	MIDIPlayer(std::string midiName, std::string soundEffectName, float basePitch, AudioManager& audioManager, float speed = 1.0f);
	void Update(float deltaTime);

	bool IsFinished() const;

	void SetPosition(glm::vec3 pos);
	void SetFollowListener(bool value);
	void SetLooping(bool toggle);
	void Reset();
private:
	static constexpr float pitchPerNote = 1.0577789506554859296792575f;	//Change in pitch per note
	
	std::vector<AudioSource> channels;
	int activeChannel = 0;
	int nChannels = 10;

	std::vector<Note> notes;
	int index = 0;
	float currentTime = 0.0f;
	bool finished = false;

	const float speed;

	bool looping = false;
};