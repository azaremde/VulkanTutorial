#ifndef __EdelVkDevice_h__
#define __EdelVkDevice_h__

#pragma once

#include "QueueFamilyIndices.h"

#include "Pch.h"

class EdelVkDevice
{
private:
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;

	VkSurfaceKHR surface;

	/** Abstract these away as well */
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	QueueFamilyIndices indices;

	static bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

	void PickPhysicalDevice(const VkInstance& instance, bool enableValidationLayers);
	void CreateLogicalDevice(const VkInstance& instance, bool enableValidationLayers);

	inline static const Avec<char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	// Fixme
	inline static const Avec<char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	bool IsDeviceSuitable(VkPhysicalDevice device);

public:
	void Init(const VkInstance& instance, bool enableValidationLayers);
	void Destroy();

	VkSurfaceKHR& GetSurface();
	VkQueue& GetGraphicsQueue();
	VkQueue& GetPresentQueue();
	QueueFamilyIndices& GetIndices();
	VkDevice& GetDevice();
	VkPhysicalDevice& GetPhysicalDevice();
};

#endif