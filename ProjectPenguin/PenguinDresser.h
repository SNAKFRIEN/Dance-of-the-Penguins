#pragma once

#include "JointAttachment.h"

#include <random>

struct Accessory
{
	Accessory(std::string name, std::string bone, std::string vertShader = "CelShader.vert", std::string fragShader = "CelShader.frag")
		:
		name(name),
		bone(bone),
		vertShader(vertShader),
		fragShader(fragShader)
	{}
	std::string name;
	std::string bone;
	std::string vertShader;
	std::string fragShader;
};

class PenguinDresser
{
public:
	PenguinDresser(std::mt19937& rng);
	//Returns vector of accessories
	//Accessories are represented by pairs of <name of model> and <name of bone to attach to>
	std::vector<Accessory> GeneratePenguinOutfit();
private:
	int Random(int min, int max);
private:
	std::mt19937& rng;
	std::uniform_int_distribution<int> randomBool;
};