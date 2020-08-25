#pragma once

#include <string>
#include <vector>

struct WAVData
{
	int fileSize;
	int chunkSize;
	short formatType;
	short channels;
	int sampleRate;
	int avgBytesPerSec;
	short bytesPerSample;
	short bitsPerSample;
	int dataSize;
	std::vector<char> data;
};

class WAVLoader
{
public:
	WAVData LoadWAV(std::string path);
};