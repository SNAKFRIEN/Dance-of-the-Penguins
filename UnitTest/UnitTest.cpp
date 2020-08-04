#include "pch.h"
#include "CppUnitTest.h"

#include "../ProjectPenguin/Window.h"
#include "../ProjectPenguin/Model.h"
#include "../ProjectPenguin/IceSkaterCollider.h"
#include "../ProjectPenguin/IceRink.h"
#include "../ProjectPenguin/Penguin.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(ModelLoading)
	{
	public:
		TEST_METHOD(ModelNotFound)
		{
			//Create a window (sets up glad etc.)
			Window window(1280, 720);

			//Store the code as a lambda
			auto func = []() {
				//All models require a reference to the transform of the object they belong to
				glm::mat4 owner(1.0f);

				//Load model with name that doesn't exist
				Model model("asdf.gltf", owner);
			};

			//Capture error message
			std::string errorMessage;
			try
			{
				func();
			}
			catch (std::exception& e)
			{
				errorMessage = e.what();
			}

			//Assert that the correct exception was thrown
			Assert::IsTrue(errorMessage.find("Failed to load model: asdf.gltf") != std::string::npos);
		}
	};
	TEST_CLASS(IceSkaterCollision)
	{
	public:
		TEST_METHOD(IceSkaterDetectsInRink)
		{
			//Check that the ice skater is able to detect that it's within the ice rink under normal circumstances
			glm::vec3 iceSkaterPos(10.0f, 0.0f, 10.0f);
			IceSkaterCollider collider(glm::translate(glm::mat4(1.0f), iceSkaterPos));
			IceRink rink(false);
			
			Assert::IsTrue(collider.IsInRink(rink));
		}
		TEST_METHOD(IceSkaterDetectsOutOfRink)
		{
			//Check that the ice skater is able to detect that it's well outside of the ice rink
			glm::vec3 iceSkaterPos(100.0f, 0.0f, 0.0f);
			IceSkaterCollider collider(glm::translate(glm::mat4(1.0f), iceSkaterPos));
			IceRink rink(false);

			Assert::IsFalse(collider.IsInRink(rink));
		}
		TEST_METHOD(IceSkaterDetectsOutOfRinkOnEdge)
		{
			//Check that the ice skater detects that it has left the ice skater when it's just across the edge
			glm::vec3 iceSkaterPos(0.0f, 0.0f, 14.5f);
			IceSkaterCollider collider(glm::translate(glm::mat4(1.0f), iceSkaterPos));
			IceRink rink(false);

			Assert::IsFalse(collider.IsInRink(rink));
		}
		TEST_METHOD(IceSkaterDetectsInRinkInCurve)
		{
			//Check that the ice skater still correctly detects that it's within the ice rink when it's just inside one of the corners
			glm::vec3 iceSkaterPos(22.0f, 0.0f, 11.0f);
			IceSkaterCollider collider(glm::translate(glm::mat4(1.0f), iceSkaterPos));
			IceRink rink(false);

			Assert::IsTrue(collider.IsInRink(rink));
		}
		TEST_METHOD(IceSkaterDetectsOutOfRinkInCurve)
		{
			//Check that the ice skater still correctly detects that it's outside the ice rink when it's just outside one of the corners
			glm::vec3 iceSkaterPos(26.0f, 0.0f, 15.0f);
			IceSkaterCollider collider(glm::translate(glm::mat4(1.0f), iceSkaterPos));
			IceRink rink(false);

			Assert::IsFalse(collider.IsInRink(rink));
		}
	};
	TEST_CLASS(PenguinCollision)
	{
	public:
		TEST_METHOD(ResolvePenguinsInSamePosition)
		{
			std::vector<Penguin> penguins;
			penguins.reserve(2);
			penguins.emplace_back(glm::vec3(1.0f, 0.0f, 2.0f), false);
			penguins.emplace_back(glm::vec3(1.0f, 0.0f, 2.0f), false);
			
			for (int i = 0; i < penguins.size(); i++)
			{
				penguins[i].Collide(i, penguins);
			}

			Assert::IsTrue(glm::length(penguins[0].GetPos() - penguins[1].GetPos()) > Penguin::minPenguinDistance);
		}
		TEST_METHOD(ResolvePartialPenguinOverlap)
		{
			std::vector<Penguin> penguins;
			penguins.reserve(2);
			penguins.emplace_back(glm::vec3(1.0f, 0.0f, 2.0f), false);
			penguins.emplace_back(glm::vec3(1.1f, 0.0f, 2.0f), false);

			for (int i = 0; i < penguins.size(); i++)
			{
				penguins[i].Collide(i, penguins);
			}

			Assert::IsTrue(glm::length(penguins[0].GetPos() - penguins[1].GetPos()) > Penguin::minPenguinDistance);
		}
	};
}
