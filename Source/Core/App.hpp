#ifndef __Core_App_hpp__
#define __Core_App_hpp__

#include "Pch.hpp"

#include "Window.hpp"
#include "Vulkan/Vulkan.hpp"

class App
{
private:
	Window window { 800, 600, "Vulkan App" };
	Vulkan vulkan;

	void MainLoop();

public:
	void Run();
};

#endif