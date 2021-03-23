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
	queueFamilyIndices = FindQueueFamilies(physDevice, surface.GetSurface());

	return queueFamilyIndices.IsComplete();
}

QueueFamilyIndices GPU::FindQueueFamilies(const VkPhysicalDevice& physDevice, const VkSurfaceKHR& surface)
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

		VkBool32 presentSupport{ false };
		vkGetPhysicalDeviceSurfaceSupportKHR(physDevice, i, surface, &presentSupport);

		if (presentSupport)
		{
			indices.presentFamily = i;
		}

		if (indices.IsComplete())
		{
			break;
		}

		i++;
	}

	return indices;
}

void GPU::CreateLogicalDevice()
{
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		queueFamilyIndices.graphicsFamily.value(),
		queueFamilyIndices.presentFamily.value()
	};

	float queuePriority{ 1.0f };

	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.emplace_back(queueCreateInfo);
	}
	//VkDeviceQueueCreateInfo queueCreateInfo{};

	VkPhysicalDeviceFeatures deviceFeatures{};
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = 0;
	createInfo.enabledLayerCount = 0;

	VulkanCheck(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device), "Failed to create logical device.");

	vkGetDeviceQueue(device, queueFamilyIndices.graphicsFamily.value(), 0, &queues.graphics);
	vkGetDeviceQueue(device, queueFamilyIndices.presentFamily.value(), 0, &queues.present);
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