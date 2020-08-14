#include "SaveFile.h"

#include "json.hpp"

void SaveFile::LoadData(std::string fileName)
{
}

void SaveFile::SaveData(std::string fileName)
{
}

void SaveFile::RemoveFile(std::string fileName)
{
}

bool SaveFile::FileExists(std::string fileName)
{
	return false;
}

int SaveFile::GetHighScore() const
{
	return 0;
}

bool SaveFile::GetTutorialCompleted() const
{
	return false;
}

void SaveFile::SetHighScore(int score)
{
}

void SaveFile::SetTutorialCompleted(bool tutorialFinished)
{
}
