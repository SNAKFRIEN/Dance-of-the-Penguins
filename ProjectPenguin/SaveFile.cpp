#include "SaveFile.h"

#include "json.hpp"

#include <fstream>
#include <iomanip>

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
		tutorialCompleted = *data.find("tutorialCompleted");
	}
	//REPLACE: warn the user if save file is not present?
}

void SaveFile::SaveData(std::string fileName)
{
	nlohmann::json data = {
		{"highScore", highScore},
		{"tutorialCompleted", tutorialCompleted}
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

bool SaveFile::GetTutorialCompleted() const
{
	return tutorialCompleted;
}

void SaveFile::SetHighScore(int inScore)
{
	highScore = inScore;
}

void SaveFile::SetTutorialCompleted(bool inTutorialCompleted)
{
	tutorialCompleted = inTutorialCompleted;
}
