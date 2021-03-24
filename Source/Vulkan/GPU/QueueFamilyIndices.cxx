#include "QueueFamilyIndices.hpp"

bool QueueFamilyIndices::IsComplete()
{
	return graphicsFamily.has_value() && presentFamily.has_value();
}

QueueFamilyIndices QueueFamilyIndices::Find(const VkPhysicalDevice& physDevice, const VkSurfaceKHR& surface)
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