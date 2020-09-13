#pragma once

#include <string>

class SaveFile
{
public:
	SaveFile();
	void LoadData(std::string fileName);
	void SaveData(std::string fileName);
	static void RemoveFile(std::string fileName);
	static bool FileExists(std::string fileName);

	int GetHighScore() const;
	bool GetTutorialCompleted() const;

	unsigned int GetShadowRes() const;
	unsigned int GetMsaaQuality() const;

	void SetHighScore(int inScore);
	void SetTutorialCompleted(bool inTutorialCompleted);
private:
	//Gameplay values
	int highScore = 0;
	bool tutorialCompleted = false;

	//Graphics settings
	unsigned int shadowResolution = 512;
	unsigned int msaa = 4;
	//TODO: various user settings, or should be included in separate file?
};