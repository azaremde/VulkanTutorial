#ifndef __Device_SwapChainSupportDetails_h__
#define __Device_SwapChainSupportDetails_h__

#pragma once

#include "Pch.h"

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR& surface);

#endif