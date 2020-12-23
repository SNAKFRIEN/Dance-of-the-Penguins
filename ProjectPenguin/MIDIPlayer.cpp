#include "MIDIPlayer.h"

#include "MIDILoader.h"
//REMOVE
//#include <iostream>
//#include "Camera.h"

MIDIPlayer::MIDIPlayer(std::string midiName, std::string soundEffectName, float basePitch, AudioManager& audioManager, float speed)
	:
	speed(speed)
{
	std::string midiPath = "Audio/";
	midiPath.append(midiName);
	MIDILoader loader;
	auto midiData = loader.LoadMIDI(midiPath);

	int noteOffset = (int)(log(basePitch) / log(pitchPerNote));


	for (auto& note : midiData.notes)
	{
		Note n;
		n.timeStamp = (((float)note.first) / 1000.0f);
		n.pitch = (float)(pow(pitchPerNote, note.second - noteOffset));
		notes.emplace_back(n);
	}

	for (int i = 0; i < nChannels; i++)
	{
		channels.emplace_back(soundEffectName, audioManager);
	}
}

void MIDIPlayer::Update(float deltaTime)
{
	currentTime += deltaTime * speed;
	while (index < notes.size() && currentTime > notes[index].timeStamp)
	{
		//REMOVE following commented line
		//std::cout << "NOTE PLAYED ====================================== " << notes[index].pitch << " TIME " << notes[index].timeStamp << std::endl;
		channels[activeChannel].SetPitch(notes[index].pitch);
		channels[activeChannel].SetVolume((1.1f * notes[index].pitch) - 0.5f);
		channels[activeChannel].Play();
		index++;
		activeChannel++;
		activeChannel %= nChannels;
	}
	finished = index >= notes.size();
	if (looping && finished)
	{
		//Loop song
		currentTime = 0.0f;
		index = 0;
	}
}

bool MIDIPlayer::IsFinished() const
{
    return finished;
}

void MIDIPlayer::SetPosition(glm::vec3 pos)
{
	for (AudioSource& channel : channels)
	{
		channel.SetPos(pos);
	}
}

void MIDIPlayer::SetLooping(bool toggle)
{
	looping = toggle;
}

void MIDIPlayer::Reset()
{
	index = 0;
	currentTime = 0.0f;
}
