#include "App.hpp"

void App::MainLoop()
{
	while (!window.ShouldClose())
	{
		window.PollEvents();

		vulkan.Draw();
	}

	vulkan.WaitForIdle();
}

void App::Run()
{
	MainLoop();
}