#ifndef __Vulkan_GPU_SwapChainSupport_hpp__
#define __Vulkan_GPU_SwapChainSupport_hpp__

#pragma once

#include "Pch.hpp"

struct SwapChainSupport
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;

	static SwapChainSupport Query(const VkPhysicalDevice& physDevice, const VkSurfaceKHR& surface);
};

#endif