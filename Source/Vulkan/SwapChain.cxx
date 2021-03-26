#include "SwapChain.hpp"

SwapChain::SwapChain(VkInstance& _instance, GPU& _gpu, Surface& _surface, Window& _window) : 
	instance{ _instance },
	gpu{ _gpu },
	surface{ _surface },
	window{ _window }
{
	CreateSwapChain();
	CreateImageViews();
}

void SwapChain::CreateSwapChain()
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

	vkGetSwapchainImagesKHR(gpu.Device(), swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(gpu.Device(), swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

void SwapChain::CreateImageViews()
{
	swapChainImageViews.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VulkanCheck(
			vkCreateImageView(gpu.Device(), &createInfo, nullptr, &swapChainImageViews[i]),
			"Failed to create image views."
		);
	}
}

void SwapChain::Destroy()
{
	for (const auto& imageView : swapChainImageViews)
	{
		vkDestroyImageView(gpu.Device(), imageView, nullptr);
	}

	vkDestroySwapchainKHR(gpu.Device(), swapChain, nullptr);
}

SwapChain::~SwapChain()
{
	Destroy();
}

const VkSwapchainKHR& SwapChain::GetSwapChain() const
{
	return swapChain;
}

const VkExtent2D& SwapChain::GetSwapChainExtent() const
{
	return swapChainExtent;
}

const VkFormat& SwapChain::GetImageFormat() const
{
	return swapChainImageFormat;
}

const std::vector<VkImageView>& SwapChain::GetSwapChainImageViews() const
{
	return swapChainImageViews;
}

const std::vector<VkImage>& SwapChain::GetSwapChainImages() const
{
	return swapChainImages;
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

//#define FORCE_MAX_PERFORMANCE

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