#include "pch.h"
#include "CppUnitTest.h"

#include "../ProjectPenguin/Window.h"
#include "../ProjectPenguin/Model.h"
#include "../ProjectPenguin/IceSkaterCollider.h"
#include "../ProjectPenguin/IceRink.h"
#include "../ProjectPenguin/Penguin.h"
#include "../ProjectPenguin/EliMath.h"
#include "../ProjectPenguin/Spawner.h"
#include "../ProjectPenguin/UserInterface.h"
#include "../ProjectPenguin/SaveFile.h"

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
			Assert::IsTrue(errorMessage.find("Failed to load model: asdf.gltf") != std::string::npos, L"The expected exception was not thrown");
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
			
			Assert::IsTrue(collider.IsInRink(rink), L"The ice skater collider incorrectly detected that it was outside the rink");
		}
		TEST_METHOD(IceSkaterDetectsOutOfRink)
		{
			//Check that the ice skater is able to detect that it's well outside of the ice rink
			glm::vec3 iceSkaterPos(100.0f, 0.0f, 0.0f);
			IceSkaterCollider collider(glm::translate(glm::mat4(1.0f), iceSkaterPos));
			IceRink rink(false);

			Assert::IsFalse(collider.IsInRink(rink), L"The ice skater collider incorrectly detected that it was inside the rink");
		}
		TEST_METHOD(IceSkaterDetectsOutOfRinkOnEdge)
		{
			//Check that the ice skater detects that it has left the ice skater when it's just across the edge
			glm::vec3 iceSkaterPos(0.0f, 0.0f, 13.6f);
			IceSkaterCollider collider(glm::translate(glm::mat4(1.0f), iceSkaterPos));
			IceRink rink(false);

			Assert::IsFalse(collider.IsInRink(rink), L"The ice skater collider incorrectly detected that it was inside the rink");
		}
		TEST_METHOD(IceSkaterDetectsInRinkInCurve)
		{
			//Check that the ice skater still correctly detects that it's within the ice rink when it's just inside one of the corners
			glm::vec3 iceSkaterPos(21.0f, 0.0f, 10.0f);
			IceSkaterCollider collider(glm::translate(glm::mat4(1.0f), iceSkaterPos));
			IceRink rink(false);

			Assert::IsTrue(collider.IsInRink(rink), L"The ice skater collider incorrectly detected that it was outside the rink");
		}
		TEST_METHOD(IceSkaterDetectsOutOfRinkInCurve)
		{
			//Check that the ice skater still correctly detects that it's outside the ice rink when it's just outside one of the corners
			glm::vec3 iceSkaterPos(24.0f, 0.0f, 13.0f);
			IceSkaterCollider collider(glm::translate(glm::mat4(1.0f), iceSkaterPos));
			IceRink rink(false);

			Assert::IsFalse(collider.IsInRink(rink), L"The ice skater collider incorrectly detected that it was inside the rink");
		}
	};
	TEST_CLASS(IceSkaterPenguinCollision)
	{
	public:
		TEST_METHOD(IceSkaterDetectsPenguinCollision)
		{
			AudioManager dummyAudioManager;

			glm::vec3 iceSkaterPos(-3.0f, 0.0f, 4.0f);
			IceSkaterCollider collider(glm::translate(glm::mat4(1.0f), iceSkaterPos));
			std::vector<Penguin> penguins;
			penguins.reserve(1);
			penguins.emplace_back(glm::vec3(-3.1f, 0.0f, 4.1f), dummyAudioManager, false);

			Assert::IsTrue(collider.IsCollidingWithPenguin(penguins), L"The ice skater collider did not detect the penguin collision");
		}
		TEST_METHOD(IceSkaterDetectsNoPenguinCollision)
		{
			AudioManager dummyAudioManager;

			glm::vec3 iceSkaterPos(5.0f, 0.0f, -4.0f);
			IceSkaterCollider collider(glm::translate(glm::mat4(1.0f), iceSkaterPos));
			std::vector<Penguin> penguins;
			penguins.reserve(3);
			penguins.emplace_back(glm::vec3(-3.1f, 0.0f, 4.1f), dummyAudioManager,  false);
			penguins.emplace_back(glm::vec3(5.0f, 0.0f, 4.0f), dummyAudioManager, false);
			penguins.emplace_back(glm::vec3(-5.0f, 0.0f, 4.0f), dummyAudioManager, false);

			Assert::IsFalse(collider.IsCollidingWithPenguin(penguins), L"The ice skater collider incorrectly detected a collision with a penguin");
		}
	};
	TEST_CLASS(PenguinToPenguinCollision)
	{
	public:
		TEST_METHOD(ResolvePenguinsInSamePosition)
		{
			AudioManager dummyAudioManager;

			std::vector<Penguin> penguins;
			penguins.reserve(2);
			penguins.emplace_back(glm::vec3(1.0f, 0.0f, 2.0f), dummyAudioManager, false);
			penguins.emplace_back(glm::vec3(1.0f, 0.0f, 2.0f), dummyAudioManager, false);

			//It may have been better to separate the ice rink collision detection from the penguin collision detection, but this should not affect the test
			IceRink dummyRink(false);

			//It should only be necessary to call collide for one of the two penguins to fully resolve the collision for both
			penguins[0].Collide(0, penguins, dummyRink);

			Assert::IsTrue(glm::length(penguins[0].GetPos() - penguins[1].GetPos()) >= Penguin::minPenguinDistance, L"The penguins were still colliding after the collision should have been resolved");
		}
		TEST_METHOD(ResolvePartialPenguinOverlap)
		{
			AudioManager dummyAudioManager;

			std::vector<Penguin> penguins;
			penguins.reserve(2);
			penguins.emplace_back(glm::vec3(1.0f, 0.0f, 2.0f), dummyAudioManager, false);
			penguins.emplace_back(glm::vec3(1.1f, 0.0f, 2.0f), dummyAudioManager, false);

			//It may have been better to separate the ice rink collision detection from the penguin collision detection, but this should not affect the test
			IceRink dummyRink(false);

			for (int i = 0; i < penguins.size(); i++)
			{
				penguins[i].Collide(i, penguins, dummyRink);
			}

			Assert::IsTrue(glm::length(penguins[0].GetPos() - penguins[1].GetPos()) >= Penguin::minPenguinDistance, L"The penguins were still colliding after the collision should have been resolved");
		}
	};
	TEST_CLASS(PenguinToRinkCollision)
	{
		TEST_METHOD(PenguinAboveRink)
		{
			AudioManager dummyAudioManager;

			std::vector<Penguin> penguins;
			penguins.reserve(1);
			penguins.emplace_back(glm::vec3(-6.0f, 0.0f, -40.0f), dummyAudioManager, false);

			IceRink rink(false);

			penguins[0].Collide(0, penguins, rink);

			glm::vec3 expectedPosition = glm::vec3(-6.0f, 0.0f, -rink.GetTop() + Penguin::minDistanceFromRinkEdges);

			Assert::AreEqual(expectedPosition.x, penguins[0].GetPos().x, L"The penguin did not resolve its collision to move to the expected position");
			Assert::AreEqual(expectedPosition.y, penguins[0].GetPos().y, L"The penguin did not resolve its collision to move to the expected position");
			Assert::AreEqual(expectedPosition.z, penguins[0].GetPos().z, L"The penguin did not resolve its collision to move to the expected position");
		}
		TEST_METHOD(PenguinOutsideBottomLeft)
		{
			AudioManager dummyAudioManager;

			std::vector<Penguin> penguins;
			penguins.reserve(1);
			penguins.emplace_back(glm::vec3(-90.0f, 0.0f, 50.0f), dummyAudioManager, false);

			IceRink rink(false);

			const glm::vec3 circleCenter = glm::vec3(-(rink.GetRight() - rink.GetCornerRadius()), 0.0f, rink.GetTop() - rink.GetCornerRadius());
			const glm::vec3 expectedPosition = glm::normalize(penguins[0].GetPos() - circleCenter) * (rink.GetCornerRadius() - Penguin::minDistanceFromRinkEdges - 0.001f) + circleCenter;

			penguins[0].Collide(0, penguins, rink);

			Assert::AreEqual(expectedPosition.x, penguins[0].GetPos().x, L"The penguin did not resolve its collision to move to the expected position");
			Assert::AreEqual(expectedPosition.y, penguins[0].GetPos().y, L"The penguin did not resolve its collision to move to the expected position");
			Assert::AreEqual(expectedPosition.z, penguins[0].GetPos().z, L"The penguin did not resolve its collision to move to the expected position");
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
			Assert::AreEqual(target.x, result.x, L"The raycast returned an incorrect result");
			Assert::AreEqual(target.y, result.y, L"The raycast returned an incorrect result");
			Assert::AreEqual(target.z, result.z, L"The raycast returned an incorrect result");
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
				abs(pClip.z) < pClip.w,
				L"The chosen spawn point was still visible on screen"
			);
		}
	};
	TEST_CLASS(UIScaling)
	{
	public:
		TEST_METHOD(CorrectlyScaleUIToAspectRatio)
		{
			//Create window with 4:3 aspect ratio
			Window dummyWindow(800, 600);
			//Create menu with 1:1 aspect ratio
			UICanvas testMenu(dummyWindow, 1.0f);
			//Create button just above the center of the screen
			testMenu.AddButton(glm::vec2(-0.4f, 0.4f), glm::vec2(0.5f, 0.1f), "MrButton");

			//Allow the menu to resize to the window
			testMenu.Update();

			//Check if the button has the correct size
			const UIButton& mrButton = testMenu.GetButton("MrButton");
			Assert::AreEqual(-0.3f, mrButton.GetLeft(), L"The button was not correctly scaled");
			Assert::AreEqual(0.375f, mrButton.GetRight(), L"The button was not correctly scaled");
			Assert::AreEqual(0.1f, mrButton.GetBottom(), L"The button was not correctly scaled");
			Assert::AreEqual(0.4f, mrButton.GetTop(), L"The button was not correctly scaled");
		}
	};
	TEST_CLASS(SaveFiles)
	{
	public:
		TEST_METHOD(CreateAndDeleteFile)
		{
			std::string fileName = "test0.json";

			//Create file
			SaveFile save;
			save.SaveData(fileName);
			Assert::IsTrue(SaveFile::FileExists(fileName), L"test0.json was likely not created");

			//Remove file
			SaveFile::RemoveFile(fileName);
			Assert::IsFalse(SaveFile::FileExists(fileName), L"test0.json was likely not deleted");
		}
		TEST_METHOD(SaveAndLoadData)
		{
			std::string fileName = "test1.json";
			int highScore = 123;
			bool tutorialFinished = true;

			//Create and save file
			SaveFile save;
			save.SetHighScore(highScore);
			save.SetTutorialCompleted(tutorialFinished);
			save.SaveData(fileName);

			//Load data into different object to be safe
			SaveFile load;
			load.SetHighScore(0);
			load.SetTutorialCompleted(false);
			load.LoadData(fileName);

			//Cleanup
			SaveFile::RemoveFile("test1.json");

			//Check that data was correctly stored and loaded
			Assert::AreEqual(highScore, load.GetHighScore(), L"high scores don't match");
			Assert::AreEqual(tutorialFinished, load.GetTutorialCompleted(), L"tutorial completed does not match");
		}
	};
}
