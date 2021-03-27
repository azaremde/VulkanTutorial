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
	bool IsBestDevice(const VkPhysicalDevice& physDevice);
	void PickPhysicalDevice();

	VkDevice device{ VK_NULL_HANDLE };
	Queues queues;
	void CreateLogicalDevice();
	void DestroyLogicalDevice();

	inline static const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	static bool CheckDeviceExtensionSupport(const VkPhysicalDevice& physDevice);

	GPU(const GPU&) = delete;
	GPU& operator=(GPU&) = delete;

public:
	void CheckDeviceProperties();

	GPU(VkInstance& _instance, Surface& _surface);
	~GPU();

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void CopyBuffer(const VkCommandPool& commandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	VkPhysicalDevice& PhysicalDevice();
	VkDevice& Device();

	Queues& GetQueues();

	SwapChainSupport& GetSwapChainSupport();
};

#endif