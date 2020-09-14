#include "PenguinDresser.h"

PenguinDresser::PenguinDresser(std::mt19937& rng)
	:
	rng(rng),
	randomBool(0, 1),
	randomChance100(0, 99)
{
}

std::vector<std::pair<std::string, std::string>> PenguinDresser::GeneratePenguinOutfit()
{
	std::vector<std::pair<std::string, std::string>> result;
	//Determine if dressed
	if ((bool)randomBool(rng))
	{
		//Choose between randomized outfit and unique penguin
		int regularFate = randomChance100(rng);
		if (regularFate < 50)	//50% chance of being unique penguin, this will be REPLACEd before release
		{
			//Hat
			if ((bool)randomBool(rng))
			{
				int hatFate = randomChance100(rng);
				if (hatFate < 25)
				{
					result.emplace_back("EarmuffsCat.gltf", "head");
				}
				else if (hatFate < 50)
				{
					result.emplace_back("EarmuffsRabbit.gltf", "head");
				}
				else if (hatFate < 75)
				{
					result.emplace_back("RussianHat.gltf", "head");
				}
				else
				{
					result.emplace_back("TrafficConeHat.gltf", "head");
				}
			}
			//Torso
			/*
			if ((bool)randomBool(rng))
			{
				int torsoFate = randomChance100(rng);
				if (torsoFate < 50)
				{
					result.emplace_back("Tutu.gltf", "torso");
				}
				else
				{
					result.emplace_back("HotDog.gltf", "torso");
				}
			}
			*/
			//Hands
			if ((bool)randomBool(rng))
			{
				//Choose hand
				if ((bool)randomBool(rng))
				{

				}
				else
				{

				}
			}
		}
		else //Is unique penguin
		{
			int uniqueFate = randomChance100(rng);
			if (uniqueFate < 17)
			{
				//Ballerina
				result.emplace_back("Tutu.gltf", "torso");
			}
			else if (uniqueFate < 33)
			{
				//HotDog
				result.emplace_back("HotDog.gltf", "torso");
				result.emplace_back("Mustard.gltf", "lower_arm.L");
				result.emplace_back("Ketchup.gltf", "lower_arm.R");
			}
			else if (uniqueFate < 50)
			{
				//MIB
				result.emplace_back("Suitcase.gltf", "lower_arm.R");
				result.emplace_back("Sunglasses.gltf", "head");
				result.emplace_back("Tie.gltf", "torso");
			}
			else if (uniqueFate < 67)
			{
				//Bobblehead
				result.emplace_back("BubbleHead.gltf", "head");
			}
			else if(uniqueFate < 83)
			{
				//Stealth
				result.emplace_back("CardboardBox.gltf", "head");
			}
			else
			{
				//King
				result.emplace_back("Cape.gltf", "torso");
				result.emplace_back("Crown.gltf", "head");
				result.emplace_back("Staff.gltf", "lower_arm.R");
			}
		}
	}

    return std::move(result);
}
