#ifndef __Vulkan_Vulkan_hpp__
#define __Vulkan_Vulkan_hpp__

#pragma once

#include "Pch.hpp"

#include "Core/Window.hpp"
#include "Surface.hpp"
#include "GPU/GPU.hpp"
#include "SwapChain.hpp"
#include "Pipeline/Pipeline.hpp"
#include "Drawing.hpp"

class Vulkan
{
private:
	Window& window;

	VkInstance instance;
	void CreateInstance();
	void DestroyInstance();

	void CreateDebugger();
	void DestroyDebugger();

	Surface* surface;
	void CreateSurface();
	void DestroySurface();

	GPU* gpu;
	void CreateGPU();
	void DestroyGPU();

	SwapChain* swapChain;
	void CreateSwapChain();
	void DestroySwapChain();

	Pipeline* pipeline;
	void CreatePipeline();
	void DestroyPipeline();

	Drawing* drawing;
	void CreateDrawing();
	void DestroyDrawing();

	Vulkan(const Vulkan&) = delete;
	Vulkan& operator=(const Vulkan&) = delete;

	std::vector<const char*> GetRequiredExtensions();

public:
	Vulkan(Window& _window);
	~Vulkan();

	void Draw();
	void WaitForIdle();

};

#endif