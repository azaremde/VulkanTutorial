#ifndef __Vulkan_GPU_QueueFamilyIndices_hpp__
#define __Vulkan_GPU_QueueFamilyIndices_hpp__

#pragma once

#include "Pch.hpp"

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool IsComplete();

	static QueueFamilyIndices Find(const VkPhysicalDevice& physDevice, const VkSurfaceKHR& surface);
};

#endif