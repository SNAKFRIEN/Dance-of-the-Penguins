#include "MIDIPlayer.h"

#include "MIDILoader.h"
//REMOVE
#include <iostream>
#include "Camera.h"

MIDIPlayer::MIDIPlayer(std::string midiName, std::string soundEffectName, float basePitch, AudioManager& audioManager, float speed)
	:
	speed(speed)
{
	MIDILoader loader;
	auto midiData = loader.LoadMIDI(midiName);

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

void MIDIPlayer::Update(float deltaTime, const Camera& camera)
{
	currentTime += deltaTime * speed;
	while (currentTime > notes[index].timeStamp && index < notes.size())
	{
		std::cout << "NOTE PLAYED ====================================== " << notes[index].pitch << " TIME " << notes[index].timeStamp << std::endl;
		channels[activeChannel].SetPos(camera.GetPos());
		channels[activeChannel].SetPitch(notes[index].pitch);
		channels[activeChannel].SetVolume((1.1f * notes[index].pitch) - 0.5f);
		channels[activeChannel].Play();
		index++;
		activeChannel++;
		activeChannel %= nChannels;
	}
	if (index >= notes.size())
	{
		//Loop song
		currentTime = 0.0f;
		index = 0;
	}
}
