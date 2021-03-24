#ifndef __Vulkan_GPU_GPU_hpp__
#define __Vulkan_GPU_GPU_hpp__

#pragma once

#include "Pch.hpp"

#include "Vulkan/Surface.hpp"

#include "SwapChainSupport.hpp"

#include "Queues.hpp"

class GPU
{
private:
	VkInstance& instance;
	Surface& surface;

	VkPhysicalDevice physicalDevice { VK_NULL_HANDLE };
	SwapChainSupport swapChainSupport{};
	void PickPhysicalDevice();
	bool IsBestDevice(const VkPhysicalDevice& physDevice);

	VkDevice device{ VK_NULL_HANDLE };
	Queues queues;
	void CreateLogicalDevice();
	void DestroyLogicalDevice();

	inline static const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	static bool CheckDeviceExtensionSupport(const VkPhysicalDevice& physDevice);

	GPU(const GPU&) = delete;
	GPU& operator=(GPU&) = delete;

public:
	GPU(VkInstance& _instance, Surface& _surface);
	~GPU();

	VkPhysicalDevice& PhysicalDevice();
	VkDevice& Device();

	Queues& GetQueues();

	SwapChainSupport& GetSwapChainSupport();
};

#endif