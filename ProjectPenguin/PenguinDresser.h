#pragma once

#include "JointAttachment.h"

#include <random>
class PenguinDresser
{
public:
	PenguinDresser(std::mt19937& rng);
	//Returns vector of accessories
	//Accessories are represented by pairs of <name of model> and <name of bone to attach to>
	std::vector<std::pair<std::string, std::string>> GeneratePenguinOutfit();
private:
	std::mt19937& rng;
	std::uniform_int_distribution<int> randomBool;
	std::uniform_int_distribution<int> randomChance100;
};