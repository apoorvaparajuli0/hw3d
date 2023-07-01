#include "App.h"

App::App()
	:
	wnd(800, 600, "The Box")
{}

int App::Go()
{
	//infinite loop that calls our window's ProcessMessages function
	//which will return an empty optional which evaluates to false here
	//when we quit, if we're not quitting then we continue an execute
	//a frame of game logic
	while (true)
	{
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}
		DoFrame();
	}
}

void App::DoFrame()
{
	wnd.Gfx().EndFrame();
}