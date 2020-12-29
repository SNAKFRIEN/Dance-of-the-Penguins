#include "PenguinDresser.h"

PenguinDresser::PenguinDresser(std::mt19937& rng)
	:
	rng(rng),
	randomBool(0, 1)
{
}

std::vector<Accessory> PenguinDresser::GeneratePenguinOutfit()
{
	std::vector<Accessory> result;

	//Determine if dressed
	if (Random(0, 3) < 3)
	{
		//Choose between common and unique penguin
		if (Random(0, 100) < 98)
		{
			int randomValue = Random(0, 8);
			switch (randomValue)
			{
			case 0:
				result.emplace_back("EarmuffsCat.gltf", "head");
				break;
			case 1:
				result.emplace_back("EarmuffsRabbit.gltf", "head");
				break;
			case 2:
				result.emplace_back("RussianHat.gltf", "head");
				break;
			case 3:
				result.emplace_back("Monocle.gltf", "head");
				break;
			case 4:
				result.emplace_back("Moustache.gltf", "head");
				break;
			case 5:
				result.emplace_back("CowboyHat.gltf", "head");
				break;
			case 6:
				result.emplace_back("Tutu.gltf", "torso");
				break;
			case 7:
				result.emplace_back("Backpack.gltf", "torso");
				break;
			case 8:
				result.emplace_back("Suitcase.gltf", "lower_arm.R");
				result.emplace_back("Sunglasses.gltf", "head");
				result.emplace_back("Tie.gltf", "torso");
				break;
			}
			assert(!result.empty());
		}
		else //Is unique penguin
		{
			int randomValue = Random(0, 7);
			switch (randomValue)
			{
			case 0:
				//Football
				result.emplace_back("Football.gltf", "lower_arm.L");
				result.emplace_back("FootballHelmet.gltf", "head");
				result.emplace_back("FootballArmor.gltf", "torso");
				break;
			case 1:
				//Bobblehead
				result.emplace_back("BubbleHead.gltf", "head", "SmoothShader.vert", "SmoothBright.frag");
				break;
			case 2:
				//Stealth
				result.emplace_back("CardboardBox.gltf", "head", "SmoothShader.vert", "SmoothBright.frag");
				break;
			case 3:
				//King
				result.emplace_back("Cape.gltf", "torso");
				result.emplace_back("Crown.gltf", "head");
				result.emplace_back("Staff.gltf", "lower_arm.R");
				break;
			case 4:
				//Chef
				result.emplace_back("ChefHat.gltf", "head");
				result.emplace_back("ChefPan.gltf", "lower_arm.L");
				break;
			case 5:
				//Dino suit
				result.emplace_back("DinoHead.gltf", "head");
				result.emplace_back("DinoBody.gltf", "torso");
				break;
			case 6:
				//Astronaut
				result.emplace_back("AstronautHelmet.gltf", "head");
				result.emplace_back("AstronautSuit.gltf", "torso");
				break;
			case 7:
				//Santa
				result.emplace_back("SantaHat.gltf", "head");
				result.emplace_back("SantaSuit.gltf", "torso");
			}
			assert(!result.empty());
		}
		assert(!result.empty());
	}

    return std::move(result);
}

int PenguinDresser::Random(int min, int max)
{
	std::uniform_int_distribution<int> randomValue(min, max);
	return randomValue(rng);
}
