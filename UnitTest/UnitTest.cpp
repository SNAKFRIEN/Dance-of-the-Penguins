#include "pch.h"
#include "CppUnitTest.h"

#include "../ProjectPenguin/Window.h"
#include "../ProjectPenguin/Model.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(ModelLoading)
	{
	public:
		TEST_METHOD(CheckStuff)
		{
			//Create a window (sets up glad etc.)
			Window window(1280, 720);

			//Store the code as a lambda
			auto func = []() {
				//All models require a reference to the transform of the object they belong to
				glm::mat4 owner(1.0f);

				//Load model with name that doesn't exist
				Model model("Goopie.gltf", owner);
			};

			try
			{
				func();
			}
			catch (std::exception& e)
			{
				Logger::WriteMessage(e.what());
			}

			//Assert that the correct exception was thrown
			Assert::ExpectException<std::invalid_argument>(func);
		}
	};
}
