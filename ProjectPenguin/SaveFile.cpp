#include "SaveFile.h"

#include "json.hpp"

#include <fstream>
#include <iomanip>

SaveFile::SaveFile()
{
	LoadData("SaveData.json");
}

void SaveFile::LoadData(std::string fileName)
{
	if (FileExists(fileName))
	{
		//Gain access to the data
		std::string filePath = "UserData/";
		filePath.append(fileName);
		std::ifstream file(filePath);
		nlohmann::json data;
		file >> data;
		
		//Load data
		highScore = *data.find("highScore");
		shadowResolution = *data.find("shadowResolution");
		msaa = *data.find("msaa");
		selectedMonitor = *data.find("selectedMonitor");
		fullScreenOn = *data.find("fullScreenOn");
	}
	//REPLACE: warn the user if save file is not present?
}

void SaveFile::SaveData(std::string fileName)
{
	nlohmann::json data = {
		{"highScore", highScore},
		{"shadowResolution", shadowResolution},
		{"msaa", msaa},
		{"selectedMonitor", selectedMonitor},
		{"fullScreenOn", fullScreenOn}
	};

	std::string filePath = "UserData/";
	filePath.append(fileName);
	std::ofstream file(filePath);
	file << std::setw(4) << data << std::endl;
}

void SaveFile::RemoveFile(std::string fileName)
{
	std::string filePath = "UserData/";
	filePath.append(fileName);
	remove(filePath.c_str());
}

bool SaveFile::FileExists(std::string fileName)
{
	std::string filePath = "UserData/";
	filePath.append(fileName);
	struct stat buffer;
	return (stat(filePath.c_str(), &buffer) == 0);
}

int SaveFile::GetHighScore() const
{
	return highScore;
}

void SaveFile::SetHighScore(int inScore)
{
	highScore = inScore;
}

unsigned int SaveFile::GetShadowRes() const
{
	return shadowResolution;
}

unsigned int SaveFile::GetMsaaQuality() const
{
	return msaa;
}

int SaveFile::GetSelectedMonitor() const
{
	return selectedMonitor;
}

bool SaveFile::GetFullScreenOn() const
{
	return fullScreenOn;
}
