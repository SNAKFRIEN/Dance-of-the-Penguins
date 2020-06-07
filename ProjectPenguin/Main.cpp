#include <Windows.h>
#include "Window.h"
#include "Game.h"

int main()
{
	try
	{
		Window window(1920, 1080);
		Game game(window);


		glEnable(GL_DEPTH_TEST);

		//Loop the game
		while (!window.IsClosing())
		{
			window.BeginFrame();

			if (window.KeyIsPressed(GLFW_KEY_ESCAPE))
			{
				window.Close();
			}

			game.Update();
			game.Draw();

			window.EndFrame();
		}
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Something's Wrong", MB_OK | MB_ICONEXCLAMATION);
	}
	return 0;
}