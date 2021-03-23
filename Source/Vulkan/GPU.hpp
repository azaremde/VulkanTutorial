#ifndef __Vulkan_GPU_hpp__
#define __Vulkan_GPU_hpp__

#pragma once

#include "Pch.hpp"

#include "Surface.hpp"

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool IsComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct Queues
{
	VkQueue graphics;
	VkQueue present;
};

class GPU
{
private:
	VkInstance& instance;
	Surface& surface;

	VkPhysicalDevice physicalDevice { VK_NULL_HANDLE };
	void PickPhysicalDevice();
	bool IsBestDevice(const VkPhysicalDevice& physDevice);
	QueueFamilyIndices queueFamilyIndices{};
	static QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& physDevice, const VkSurfaceKHR& surface);

	VkDevice device{ VK_NULL_HANDLE };
	Queues queues;
	void CreateLogicalDevice();
	void DestroyLogicalDevice();

	GPU(const GPU&) = delete;
	GPU& operator=(GPU&) = delete;

public:
	GPU(VkInstance& _instance, Surface& _surface);
	~GPU();

	VkPhysicalDevice& PhysicalDevice();
	VkDevice& Device();
};

#endif