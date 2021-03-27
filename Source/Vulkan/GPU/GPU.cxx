#include "GPU.hpp"

#include <set>

GPU::GPU(VkInstance& _instance, Surface& _surface) : instance{ _instance }, surface{ _surface }
{
	PickPhysicalDevice();
	CreateLogicalDevice();
}

uint32_t GPU::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
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

void GPU::CheckDeviceProperties()
{
	queues.familyIndices = QueueFamilyIndices::Find(physicalDevice, surface.GetSurface());
	swapChainSupport = SwapChainSupport::Query(physicalDevice, surface.GetSurface());
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

void GPU::CopyBuffer(const VkCommandPool& commandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(queues.graphics, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queues.graphics);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void GPU::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(device, buffer, bufferMemory, 0);
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