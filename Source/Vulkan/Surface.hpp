#ifndef __Vulkan_Surface_hpp__
#define __Vulkan_Surface_hpp__

#pragma once

#include "Pch.hpp"

#include "Core/Window.hpp"

class Surface
{
private:
	VkInstance& instance;
	Window& window;

	VkSurfaceKHR surface;

	Surface(const Surface&) = delete;
	Surface& operator=(Surface&) = delete;

public:
	Surface(VkInstance& _instance, Window& _window);
	~Surface();

	VkSurfaceKHR& GetSurface();
};

#endif