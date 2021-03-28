#ifndef __Core_App_hpp__
#define __Core_App_hpp__

#pragma once

#include "Window.hpp"
#include "AstrumVK/AstrumVK.hpp"

class App
{
private:
	Window window{ 1024, 768, "Learning Vulkan" };
    AstrumVK vk{ window };

public:
	void run();
};

#endif