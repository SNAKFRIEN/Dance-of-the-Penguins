#pragma once

#include <string>

class SaveFile
{
public:
	void LoadData(std::string fileName);
	void SaveData(std::string fileName);
	static void RemoveFile(std::string fileName);
	static bool FileExists(std::string fileName);

	int GetHighScore() const;
	bool GetTutorialCompleted() const;

	void SetHighScore(int score);
	void SetTutorialCompleted(bool tutorialFinished);
private:
	int highScore = 0;
	bool tutorialCompleted = false;
	//TODO: various user settings, or should be included in separate file?
};