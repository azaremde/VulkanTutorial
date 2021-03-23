#ifndef __Vulkan_GPU_hpp__
#define __Vulkan_GPU_hpp__

#pragma once

#include "Pch.hpp"

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;

	bool IsComplete()
	{
		return graphicsFamily.has_value();
	}
};

struct Queues
{
	VkQueue graphics;
};

class GPU
{
private:
	VkInstance& instance;

	VkPhysicalDevice physicalDevice { VK_NULL_HANDLE };
	void PickPhysicalDevice();
	bool IsBestDevice(const VkPhysicalDevice& physDevice);
	QueueFamilyIndices queueFamilyIndices{};
	static QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& physDevice);

	VkDevice device{ VK_NULL_HANDLE };
	Queues queues;
	void CreateLogicalDevice();
	void DestroyLogicalDevice();

	GPU(const GPU&) = delete;
	GPU& operator=(GPU&) = delete;

public:
	GPU(VkInstance& _instance);
	~GPU();

	VkPhysicalDevice& PhysicalDevice();
	VkDevice& Device();
};

#endif