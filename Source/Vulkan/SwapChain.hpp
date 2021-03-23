#ifndef __Vulkan_SwapChain_hpp__
#define __Vulkan_SwapChain_hpp__

#pragma once

#include "Pch.hpp"

#include "Core/Window.hpp"

class SwapChain
{
private:
	VkInstance& instance;
	Window& window;

	//VkSurfaceKHR surface;
	//void CreateSurface();

	SwapChain(const SwapChain&) = delete;
	SwapChain& operator=(SwapChain&) = delete;

public:
	SwapChain(VkInstance& _instance, Window& _window);
	~SwapChain();
};

#endif