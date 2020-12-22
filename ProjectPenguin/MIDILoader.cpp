#include "MIDILoader.h"

#include <fstream>
#include <sstream>
#include <algorithm>
//REMOVE (from all files now that I think about it)
#include <iostream>

MIDIData MIDILoader::LoadMIDI(std::string path)
{
	MIDIData result;

	std::ifstream file(path, std::ios::binary);

	if (!file.is_open())
	{
		std::stringstream errorMessage;
		errorMessage << path << " could not be loaded because the file could not be opened";
		throw std::exception(errorMessage.str().c_str());
	}

	uint32_t buffer32 = 0;
	uint32_t buffer16 = 0;

	//Read MIDI header (fixed size)
	file.read((char*)&buffer32, sizeof(uint32_t));
	uint32_t fileID = Swap32(buffer32);
	file.read((char*)&buffer32, sizeof(uint32_t));
	uint32_t headerLength = Swap32(buffer32);
	file.read((char*)&buffer16, sizeof(uint16_t));
	uint16_t format = Swap16(buffer16);
	file.read((char*)&buffer16, sizeof(uint16_t));
	uint16_t nTrackChunks = Swap16(buffer16);
	file.read((char*)&buffer16, sizeof(uint16_t));
	uint16_t division = Swap16(buffer16);

	//Loop through tracks
	for (int i = 0; i < nTrackChunks; i++)
	{
		std::cout << "---NEW TRACK---" << std::endl;

		//Read track header
		file.read((char*)&buffer32, sizeof(uint32_t));
		uint32_t trackID = Swap32(buffer32);
		file.read((char*)&buffer32, sizeof(uint32_t));
		uint32_t trackLength = Swap32(buffer32);

		bool finished = false;	//Signifies end of track
		uint8_t lastValidEventType = 0;
		uint32_t currentTime = 0;

		while (!file.eof() && !finished)
		{
			uint32_t timeDelta = 0;
			uint8_t eventType = 0;

			timeDelta = ReadValue(file);
			eventType = file.get();
			currentTime += timeDelta;

			if (eventType < 0x80)
			{
				eventType = lastValidEventType;
				file.seekg(-1, std::ios_base::cur);
			}

			if ((eventType & 0xF0) == EventName::VoiceNoteOff)
			{
				lastValidEventType = eventType;
				uint8_t channel = eventType & 0x0F;
				uint8_t noteID = file.get();
				uint8_t noteVelocity = file.get();
			}
			else if ((eventType & 0xF0) == EventName::VoiceNoteOn)
			{
				lastValidEventType = eventType;
				uint8_t channel = eventType & 0x0F;
				uint8_t noteID = file.get();
				uint8_t noteVelocity = file.get();
				if (noteVelocity > 0)
				{
					result.notes.push_back({ currentTime, noteID });
				}
			}
			else if ((eventType & 0xF0) == EventName::VoiceAftertouch)
			{
				lastValidEventType = eventType;
				uint8_t channel = eventType & 0x0F;
				uint8_t noteID = file.get();
				uint8_t noteVelocity = file.get();
			}
			else if ((eventType & 0xF0) == EventName::VoiceControlChange)
			{
				lastValidEventType = eventType;
				uint8_t channel = eventType & 0x0F;
				uint8_t controlID = file.get();
				uint8_t controlValue = file.get();
			}
			else if ((eventType & 0xF0) == EventName::VoiceProgramChange)
			{
				lastValidEventType = eventType;
				uint8_t channel = eventType & 0x0F;
				uint8_t programID = file.get();
			}
			else if ((eventType & 0xF0) == EventName::VoiceChannelPressure)
			{
				lastValidEventType = eventType;
				uint8_t channel = eventType & 0x0F;
				uint8_t channelPressure = file.get();
			}
			else if ((eventType & 0xF0) == EventName::VoicePitchBend)
			{
				lastValidEventType = eventType;
				uint8_t channel = eventType & 0x0F;
				uint8_t LS7B = file.get();
				uint8_t MS7B = file.get();
			}
			else if ((eventType & 0xF0) == EventName::SystemExclusive)
			{
				if (eventType == 0xF0)
				{
					//System exclusive message begin
					std::cout << "System exclusive begin: " << ReadString(file, ReadValue(file)) << std::endl;
				}
				else if (eventType == 0xF7)
				{
					//System exclusive message end
					std::cout << "System exclusive end: " << ReadString(file, ReadValue(file)) << std::endl;
				}
				else if (eventType == 0xFF)
				{
					//Meta message
					uint8_t type = file.get();
					uint8_t length = ReadValue(file);
					switch (type)
					{
					case MetaSequence:
						std::cout << "Sequence Number: " << file.get() << file.get() << std::endl;
						break;
					case MetaText:
						std::cout << "Text: " << ReadString(file, length) << std::endl;
						break;
					case MetaCopyright:
						std::cout << "Copyright: " << ReadString(file, length) << std::endl;
						break;
					case MetaTrackName:
						std::cout << "Track Name: " << ReadString(file, length) << std::endl;
						break;
					case MetaInstrumentName:
						std::cout << "Instrument Name: " << ReadString(file, length) << std::endl;
						break;
					case MetaLyrics:
						std::cout << "Lyrics: " << ReadString(file, length) << std::endl;
						break;
					case MetaMarker:
						std::cout << "Marker: " << ReadString(file, length) << std::endl;
						break;
					case MetaCuePoint:
						std::cout << "Cue: " << ReadString(file, length) << std::endl;
						break;
					case MetaChannelPrefix:
						std::cout << "Prefix: " << file.get() << std::endl;
						break;
					case MetaEndOfTrack:
						finished = true;
						break;
					case MetaSetTempo:
					{
						uint32_t tempo = 0;
						uint32_t bpm = 0;
						// Tempo is in microseconds per quarter note
						if (tempo == 0)
						{
							(tempo |= (file.get() << 16));
							(tempo |= (file.get() << 8));
							(tempo |= (file.get() << 0));
							bpm = (60000000 / tempo);
							std::cout << "Tempo: " << tempo << " (" << bpm << "bpm)" << std::endl;
						}
					}
						break;
					case MetaSMPTEOffset:
						std::cout << "SMPTE: H:" << file.get() << " M:" << file.get() << " S:" << file.get() << " FR:" << file.get() << " FF:" << file.get() << std::endl;
						break;
					case MetaTimeSignature:
						std::cout << "Time Signature: " << file.get() << "/" << (2 << file.get()) << std::endl;
						std::cout << "ClocksPerTick: " << file.get() << std::endl;

						// A MIDI "Beat" is 24 ticks, so specify how many 32nd notes constitute a beat
						std::cout << "32per24Clocks: " << file.get() << std::endl;
						break;
					case MetaKeySignature:
						std::cout << "Key Signature: " << file.get() << std::endl;
						std::cout << "Minor Key: " << file.get() << std::endl;
						break;
					case MetaSequencerSpecific:
						std::cout << "Sequencer Specific: " << ReadString(file, length) << std::endl;
						break;
					default:
						std::cout << "Unrecognised MetaEvent: " << type << std::endl;
					}
				}
			}
			else
			{
				std::cout << "Unrecognized event byte: " << eventType << std::endl;
			}
		}
	}
	
	std::sort(result.notes.begin(), result.notes.end(), [](std::pair<uint32_t, uint8_t> left, std::pair<uint32_t, uint8_t> right)
		{
			return left.first < right.first;
		});
	return result;
}

uint32_t MIDILoader::Swap32(uint32_t n)
{
	return (((n >> 24) & 0xff) | ((n << 8) & 0xff0000) | ((n >> 8) & 0xff00) | ((n << 24) & 0xff000000));
}

uint16_t MIDILoader::Swap16(uint16_t n)
{
	return ((n >> 8) | (n << 8));
}

std::string MIDILoader::ReadString(std::ifstream& file, int nLength)
{
	std::string s;
	for (int i = 0; i < nLength; i++)
	{
		s += file.get();
	}
	return s;
}

uint32_t MIDILoader::ReadValue(std::ifstream& file)
{
	uint32_t value = 0;
	uint32_t byteBuffer = 0;

	//Read byte
	value = file.get();
	//if MSB is set, read another byte
	if (value & 0x80)
	{
		//Extract bottom 7 bits of read byte
		value &= 0x7F;
		
		do
		{
			//Read next byte
			byteBuffer = file.get();
			//Construct value by setting bottom 7 bits, then shifting 7 bits
			value = (value << 7) | (byteBuffer & 0x7F);

		} while (byteBuffer & 0x80);	//Loop while byteBuffer MSB is 1
	}

	return value;
}
