#ifndef __Vulkan_GPU_Queues_hpp__
#define __Vulkan_GPU_Queues_hpp__

#pragma once

#include "Pch.hpp"

#include "QueueFamilyIndices.hpp"

struct Queues
{
	VkQueue graphics;
	VkQueue present;

	QueueFamilyIndices familyIndices{};
};

#endif