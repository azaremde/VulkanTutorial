#ifndef __Vulkan_SwapChain_hpp__
#define __Vulkan_SwapChain_hpp__

#pragma once

#include "Pch.hpp"

#include "Core/Window.hpp"

#include "Vulkan/GPU/GPU.hpp"
#include "Vulkan/Surface.hpp"

class SwapChain
{
private:
	VkInstance& instance;

	GPU& gpu;
	Surface& surface;
	Window& window;

	VkSwapchainKHR swapChain;

	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	SwapChain(const SwapChain&) = delete;
	SwapChain& operator=(SwapChain&) = delete;

public:
	SwapChain(VkInstance& _instance, GPU& _gpu, Surface& _surface, Window& _window);
	~SwapChain();
};

namespace SwapChainUtil
{
	static VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	static VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	static VkExtent2D ChooseSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities, Window& window);
}

#endif