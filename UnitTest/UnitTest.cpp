#include "pch.h"
#include "CppUnitTest.h"

#include "../ProjectPenguin/Window.h"
#include "../ProjectPenguin/Model.h"
#include "../ProjectPenguin/IceSkaterCollider.h"
#include "../ProjectPenguin/IceRink.h"
#include "../ProjectPenguin/Penguin.h"
#include "../ProjectPenguin/EliMath.h"
#include "../ProjectPenguin/Spawner.h"

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
	TEST_CLASS(IceSkaterRinkDetection)
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
			glm::vec3 iceSkaterPos(0.0f, 0.0f, 13.6f);
			IceSkaterCollider collider(glm::translate(glm::mat4(1.0f), iceSkaterPos));
			IceRink rink(false);

			Assert::IsFalse(collider.IsInRink(rink));
		}
		TEST_METHOD(IceSkaterDetectsInRinkInCurve)
		{
			//Check that the ice skater still correctly detects that it's within the ice rink when it's just inside one of the corners
			glm::vec3 iceSkaterPos(21.0f, 0.0f, 10.0f);
			IceSkaterCollider collider(glm::translate(glm::mat4(1.0f), iceSkaterPos));
			IceRink rink(false);

			Assert::IsTrue(collider.IsInRink(rink));
		}
		TEST_METHOD(IceSkaterDetectsOutOfRinkInCurve)
		{
			//Check that the ice skater still correctly detects that it's outside the ice rink when it's just outside one of the corners
			glm::vec3 iceSkaterPos(24.0f, 0.0f, 13.0f);
			IceSkaterCollider collider(glm::translate(glm::mat4(1.0f), iceSkaterPos));
			IceRink rink(false);

			Assert::IsFalse(collider.IsInRink(rink));
		}
	};
	TEST_CLASS(IceSkaterPenguinCollision)
	{
	public:
		TEST_METHOD(IceSkaterDetectsPenguinCollision)
		{
			glm::vec3 iceSkaterPos(-3.0f, 0.0f, 4.0f);
			IceSkaterCollider collider(glm::translate(glm::mat4(1.0f), iceSkaterPos));
			std::vector<Penguin> penguins;
			penguins.reserve(1);
			penguins.emplace_back(glm::vec3(-3.1f, 0.0f, 4.1f), false);

			Assert::IsTrue(collider.IsCollidingWithPenguin(penguins));
		}
		TEST_METHOD(IceSkaterDetectsNoPenguinCollision)
		{
			glm::vec3 iceSkaterPos(5.0f, 0.0f, -4.0f);
			IceSkaterCollider collider(glm::translate(glm::mat4(1.0f), iceSkaterPos));
			std::vector<Penguin> penguins;
			penguins.reserve(3);
			penguins.emplace_back(glm::vec3(-3.1f, 0.0f, 4.1f), false);
			penguins.emplace_back(glm::vec3(5.0f, 0.0f, 4.0f), false);
			penguins.emplace_back(glm::vec3(-5.0f, 0.0f, 4.0f), false);

			Assert::IsFalse(collider.IsCollidingWithPenguin(penguins));
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

			//It may have been better to separate the ice rink collision detection from the penguin collision detection, but this should not affect the test
			IceRink dummyRink(false);

			//It should only be necessary to call collide for one of the two penguins to fully resolve the collision for both
			penguins[0].Collide(0, penguins, dummyRink);
			
			Logger::WriteMessage(std::to_string(glm::length(penguins[0].GetPos() - penguins[1].GetPos())).c_str());

			Assert::IsTrue(glm::length(penguins[0].GetPos() - penguins[1].GetPos()) >= Penguin::minPenguinDistance);
		}
		TEST_METHOD(ResolvePartialPenguinOverlap)
		{
			std::vector<Penguin> penguins;
			penguins.reserve(2);
			penguins.emplace_back(glm::vec3(1.0f, 0.0f, 2.0f), false);
			penguins.emplace_back(glm::vec3(1.1f, 0.0f, 2.0f), false);

			//It may have been better to separate the ice rink collision detection from the penguin collision detection, but this should not affect the test
			IceRink dummyRink(false);

			for (int i = 0; i < penguins.size(); i++)
			{
				penguins[i].Collide(i, penguins, dummyRink);
			}

			Assert::IsTrue(glm::length(penguins[0].GetPos() - penguins[1].GetPos()) >= Penguin::minPenguinDistance);
		}
	};
	TEST_CLASS(Math)
	{
	public:
		TEST_METHOD(RayCastToFloor)
		{
			glm::vec3 origin(17.0f, 38.0f, -11.0f);
			glm::vec3 target(29.0f, 0.0f, -11.0f);
			glm::vec3 result = EliMath::IntersectFloor(origin, target - origin);
			Assert::AreEqual(target.x, result.x);
			Assert::AreEqual(target.y, result.y);
			Assert::AreEqual(target.z, result.z);
		}
	};
	TEST_CLASS(Spawns)
	{
	public:
		TEST_METHOD(SpawnOffScreen)
		{
			Spawner spawner;
			glm::vec3 cameraPos(0.0f, 10.0f, 10.0f);
			glm::vec3 cameraLookAtPoint(0.0f);
			float fov = 45.0f;
			int seed = 123;
			glm::vec3 spawnPosition = spawner.FindOffScreenSpawnPoint(cameraPos, cameraLookAtPoint, fov, 0.001f);
			//Check that point is not in view
			glm::vec4 pClip = glm::perspective(glm::radians(fov), 16.0f / 9.0f, 0.1f, 200.0f)
				* glm::lookAt(cameraPos, cameraLookAtPoint, glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::vec4(spawnPosition, 1.0f);
			Assert::IsFalse(
				abs(pClip.x) < pClip.w &&
				abs(pClip.y) < pClip.w &&
				abs(pClip.z) < pClip.w
			);
		}
	};
}
