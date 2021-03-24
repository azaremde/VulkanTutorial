#include "SwapChain.hpp"

SwapChain::SwapChain(VkInstance& _instance, GPU& _gpu, Surface& _surface, Window& _window) : 
	instance{ _instance },
	gpu{ _gpu },
	surface{ _surface },
	window{ _window }
{
	SwapChainSupport swapChainSupport = gpu.GetSwapChainSupport();

	VkSurfaceFormatKHR surfaceFormat = SwapChainUtil::ChooseSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = SwapChainUtil::ChoosePresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = SwapChainUtil::ChooseSwapChainExtent(swapChainSupport.capabilities, window);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface.GetSurface(); 
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = { gpu.GetQueues().familyIndices.graphicsFamily.value(), gpu.GetQueues().familyIndices.presentFamily.value() };

	if (gpu.GetQueues().familyIndices.graphicsFamily != gpu.GetQueues().familyIndices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	VulkanCheck(
		vkCreateSwapchainKHR(gpu.Device(), &createInfo, nullptr, &swapChain),
		"Failed to create swap chain."
	);
}

SwapChain::~SwapChain()
{
	vkDestroySwapchainKHR(gpu.Device(), swapChain, nullptr);
}

namespace SwapChainUtil
{
	VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
#ifdef FORCE_MAX_PERFORMANCE
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}
#endif

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D ChooseSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities, Window& window)
	{
		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		}
		else {
			int width, height;
			window.QueryFramebufferSize(&width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}
}