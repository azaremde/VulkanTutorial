#include "GPU.hpp"

#include <set>

GPU::GPU(VkInstance& _instance, Surface& _surface) : instance{ _instance }, surface{ _surface }
{
	PickPhysicalDevice();
	CreateLogicalDevice();
}

GPU::~GPU()
{
	DestroyLogicalDevice();
}

void GPU::PickPhysicalDevice()
{
	uint32_t gpuCount{ 0 };

	vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);

	if (gpuCount == 0)
	{
		throw std::runtime_error("Failed to find GPUs with Vulkan support.");
	}

	std::vector<VkPhysicalDevice> physicalDevices(gpuCount);

	vkEnumeratePhysicalDevices(instance, &gpuCount, physicalDevices.data());

	for (const auto& physDevice : physicalDevices)
	{
		if (IsBestDevice(physDevice))
		{
			physicalDevice = physDevice;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Failed to find a suitable GPU.");
	}
}

bool GPU::IsBestDevice(const VkPhysicalDevice& physDevice)
{
	queues.familyIndices = QueueFamilyIndices::Find(physDevice, surface.GetSurface());

	bool swapChainAdequate{ false };
	bool extensionsSupported = CheckDeviceExtensionSupport(physDevice);

	if (extensionsSupported)
	{
		swapChainSupport = SwapChainSupport::Query(physDevice, surface.GetSurface());
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return queues.familyIndices.IsComplete() && extensionsSupported && swapChainAdequate;
}

bool GPU::CheckDeviceExtensionSupport(const VkPhysicalDevice& physDevice)
{
	uint32_t extensionCount{ 0 };
	vkEnumerateDeviceExtensionProperties(physDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(physDevice, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

void GPU::CreateLogicalDevice()
{
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		queues.familyIndices.graphicsFamily.value(),
		queues.familyIndices.presentFamily.value()
	};

	float queuePriority{ 1.0f };

	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queues.familyIndices.graphicsFamily.value();
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.emplace_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	createInfo.enabledLayerCount = 0;

	VulkanCheck(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device), "Failed to create logical device.");

	vkGetDeviceQueue(device, queues.familyIndices.graphicsFamily.value(), 0, &queues.graphics);
	vkGetDeviceQueue(device, queues.familyIndices.presentFamily.value(), 0, &queues.present);
}

void GPU::DestroyLogicalDevice()
{
	vkDestroyDevice(device, nullptr);
}

VkPhysicalDevice& GPU::PhysicalDevice()
{
	return physicalDevice;
}

VkDevice& GPU::Device()
{
	return device;
}

Queues& GPU::GetQueues()
{
	return queues;
}

SwapChainSupport& GPU::GetSwapChainSupport()
{
	return swapChainSupport;
}