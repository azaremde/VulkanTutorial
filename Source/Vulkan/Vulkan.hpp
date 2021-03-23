#ifndef __Vulkan_Vulkan_hpp__
#define __Vulkan_Vulkan_hpp__

#pragma once

#include "Pch.hpp"

#include "GPU.hpp"

class Vulkan
{
private:
	VkInstance instance;
	void CreateInstance();
	void DestroyInstance();

	void CreateDebugger();
	void DestroyDebugger();

	GPU* gpu;
	void CreateGPU();
	void DestroyGPU();

	Vulkan(const Vulkan&) = delete;
	Vulkan& operator=(const Vulkan&) = delete;

	std::vector<const char*> GetRequiredExtensions();

public:

	Vulkan();
	~Vulkan();

};

#endif