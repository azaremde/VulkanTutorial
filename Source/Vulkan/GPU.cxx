#include "GPU.hpp"

GPU::GPU(VkInstance& _instance) : instance{_instance}
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
	queueFamilyIndices = FindQueueFamilies(physDevice);

	return queueFamilyIndices.IsComplete();
}

QueueFamilyIndices GPU::FindQueueFamilies(const VkPhysicalDevice& physDevice)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount{ 0 };
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, queueFamilies.data());

	uint32_t i{ 0 };
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}
		i++;
	}

	return indices;
}

void GPU::CreateLogicalDevice()
{
	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	queueCreateInfo.queueCount = 1;
	float queuePriority{ 1.0f };
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures{};
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = 0;
	createInfo.enabledLayerCount = 0;

	VulkanCheck(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device), "Failed to create logical device.");
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