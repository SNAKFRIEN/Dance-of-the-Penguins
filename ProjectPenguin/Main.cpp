#include <Windows.h>
#include "Window.h"
#include "Game.h"

int main()
{
	try
	{
		Window window(1920, 1080, "Dance of the Penguins");
		Game game(window);

		//Loop the game
		while (!window.IsClosing())
		{
			window.BeginFrame();

			game.Update();
			game.Draw();

			if (game.ReadyToQuit())
			{
				window.Close();
			}

			window.EndFrame();
		}
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Something's Wrong", MB_OK | MB_ICONEXCLAMATION);
	}
	return 0;
}