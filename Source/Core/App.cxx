#include "App.hpp"

void App::MainLoop()
{
	while (!window.ShouldClose())
	{
		window.PollEvents();
	}
}

void App::Run()
{
	MainLoop();
}