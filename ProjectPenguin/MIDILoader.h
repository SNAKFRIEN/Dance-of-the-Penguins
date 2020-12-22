#pragma once

/*This class should probably not be used outside of this project, but to learn more about how it was made, 
look here: https://www.youtube.com/watch?v=040BKtnDdg0&t=2406s&ab_channel=javidx9 */


#include <string>
#include <vector>

struct MIDIData
{
	//Vector of notes (note consists of float timeStamp and uint8_t noteID) sorted by timeStamp
	std::vector<std::pair<uint32_t, uint8_t>> notes;
};

class MIDILoader
{
private:
	enum EventName : uint8_t
	{
		VoiceNoteOff = 0x80,
		VoiceNoteOn = 0x90,
		VoiceAftertouch = 0xA0,
		VoiceControlChange = 0xB0,
		VoiceProgramChange = 0xC0,
		VoiceChannelPressure = 0xD0,
		VoicePitchBend = 0xE0,
		SystemExclusive = 0xF0,
	};

	enum MetaEventName : uint8_t	//These are system specific events
	{
		MetaSequence = 0x00,
		MetaText = 0x01,
		MetaCopyright = 0x02,
		MetaTrackName = 0x03,
		MetaInstrumentName = 0x04,
		MetaLyrics = 0x05,
		MetaMarker = 0x06,
		MetaCuePoint = 0x07,
		MetaChannelPrefix = 0x20,
		MetaEndOfTrack = 0x2F,
		MetaSetTempo = 0x51,
		MetaSMPTEOffset = 0x54,
		MetaTimeSignature = 0x58,
		MetaKeySignature = 0x59,
		MetaSequencerSpecific = 0x7F,
	};
public:
	MIDIData LoadMIDI(std::string path);
private:
	//Helper functions

	//Swaps byte order of 32-bit integer
	uint32_t Swap32(uint32_t n);
	//Swaps byte order of 16-bit integer
	uint16_t Swap16(uint16_t n);
	//Read nLength bytes from the file stream into a string
	std::string ReadString(std::ifstream& file, int nLength);
	//Read a value from the file stream (midi only uses 7 bits per byte)
	uint32_t ReadValue(std::ifstream& file);
};