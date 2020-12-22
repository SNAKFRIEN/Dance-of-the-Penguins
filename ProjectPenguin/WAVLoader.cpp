#include "WAVLoader.h"

#include <fstream>
#include <sstream>

WAVData WAVLoader::LoadWAV(std::string path)
{
	WAVData result;

	//Access file via ifstream
	std::ifstream file(path, std::ios::binary);

	if (!file.is_open())
	{
		std::stringstream errorMessage;
		errorMessage << path << " could not be loaded because the file could not be opened";
		throw std::exception(errorMessage.str().c_str());
	}

	std::stringstream fileStringStream;
	fileStringStream << file.rdbuf();
	std::string fileString = fileStringStream.str();

	if (fileString.substr(0, 4) != "RIFF")
	{
		std::stringstream errorMessage;
		errorMessage << path << " could not be loaded because it's not a RIFF file";
		throw std::exception(errorMessage.str().c_str());
	}

	result.fileSize = *((int*)&fileString[4]);

	if (fileString.substr(8, 4) != "WAVE")
	{
		std::stringstream errorMessage;
		errorMessage << path << " could not be loaded because it's not a WAVE file";
		throw std::exception(errorMessage.str().c_str());
	}

	if (fileString.substr(12, 4) != "fmt ")
	{
		std::stringstream errorMessage;
		errorMessage << path << " could not be loaded because it does not contain an fmt";
		throw std::exception(errorMessage.str().c_str());
	}

	result.chunkSize = *((int*)&fileString[16]);

	result.formatType = *((short*)&fileString[20]);

	result.channels = *((short*)&fileString[22]);

	result.sampleRate = *((int*)&fileString[24]);

	result.avgBytesPerSec = *((int*)&fileString[28]);

	result.bytesPerSample = *((short*)&fileString[32]);

	result.bitsPerSample = *((short*)&fileString[34]);

	//find data start
	const size_t dataOffset = fileString.find("data") + 8;
	const size_t dataSizeOffset = dataOffset - 4;

	result.dataSize = *((int*)&fileString[dataSizeOffset]);

	result.data.reserve(result.dataSize);
	for (int i = 0; i < result.dataSize; i++)
	{
		result.data.push_back(fileString[dataOffset + i]);
	}

	return result;
}
