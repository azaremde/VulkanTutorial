#ifndef __SwapChain_EdelVkSwapChain_h__
#define __SwapChain_EdelVkSwapChain_h__

#pragma once

#include "Pch.h"

#include "Device/EdelVkDevice.h"

class EdelVkSwapChain
{
private:
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkViewport viewport{};
	VkRect2D scissor{};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		Avec<VkSurfaceFormatKHR> formats;
		Avec<VkPresentModeKHR> presentModes;
	};

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const Avec<VkSurfaceFormatKHR>& availableFormats)
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

	VkPresentModeKHR chooseSwapPresentMode(const Avec<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
		{
			return capabilities.currentExtent;
		}
		else
		{
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR& surface)
	{
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

public:
	void CreateSwapChain(EdelVkDevice& device, GLFWwindow* window)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device.GetPhysicalDevice(), device.GetSurface());

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		{
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = device.GetSurface();
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		uint32_t queueFamilyIndices[] = { device.GetIndices().graphicsFamily.value(), device.GetIndices().presentFamily.value() };

		if (device.GetIndices().graphicsFamily != device.GetIndices().presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(device.GetDevice(), &createInfo, nullptr, &swapChain) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create swap chain.");
		}

		vkGetSwapchainImagesKHR(device.GetDevice(), swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device.GetDevice(), swapChain, &imageCount, swapChainImages.data());

		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}

	void CreateImageViews(VkDevice& device)
	{
		EdelVkSwapChain& sc = *this;

		sc.GetSwapChainImageViews().resize(sc.GetSwapChainImages().size());

		for (size_t i = 0; i < sc.GetSwapChainImages().size(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = sc.GetSwapChainImages()[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = sc.GetSwapChainImageFormat();

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(device, &createInfo, nullptr, &sc.GetSwapChainImageViews()[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create image views.");
			}
		}
	}

	void CreateFramebuffers(VkRenderPass& renderPass, VkDevice& device)
	{
		EdelVkSwapChain& sc = *this;

		sc.GetSwapChainFramebuffers().resize(sc.GetSwapChainImageViews().size());

		for (size_t i = 0; i < sc.GetSwapChainImageViews().size(); i++)
		{
			VkImageView attachments[] = {
				sc.GetSwapChainImageViews()[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = sc.GetSwapChainExtent().width;
			framebufferInfo.height = sc.GetSwapChainExtent().height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &sc.GetSwapChainFramebuffers()[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create framebuffer");
			}
		}
	}

	void Clean(VkDevice& device)
	{
		EdelVkSwapChain& sc = *this;

		for (size_t i = 0; i < sc.GetSwapChainFramebuffers().size(); i++)
		{
			vkDestroyFramebuffer(device, sc.GetSwapChainFramebuffers()[i], nullptr);
		}

		for (size_t i = 0; i < sc.GetSwapChainImageViews().size(); i++)
		{
			vkDestroyImageView(device, sc.GetSwapChainImageViews()[i], nullptr);
		}

		vkDestroySwapchainKHR(device, sc.GetSwapChain(), nullptr);
	}

	inline VkSwapchainKHR& GetSwapChain()
	{
		return swapChain;
	}

	inline std::vector<VkImage>& GetSwapChainImages()
	{
		return swapChainImages;
	}

	inline VkFormat& GetSwapChainImageFormat()
	{
		return swapChainImageFormat;
	}

	inline VkExtent2D& GetSwapChainExtent()
	{
		return swapChainExtent;
	}

	inline std::vector<VkImageView>& GetSwapChainImageViews()
	{
		return swapChainImageViews;
	}

	inline std::vector<VkFramebuffer>& GetSwapChainFramebuffers()
	{
		return swapChainFramebuffers;
	}

	inline VkViewport& GetViewport()
	{
		return viewport;
	}

	void SetViewport()
	{
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapChainExtent.width);
		viewport.height = static_cast<float>(swapChainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.minDepth = 1.0f;
	}

	void SetScissor()
	{
		scissor.offset = { 0, 0 };
		scissor.extent = GetSwapChainExtent();
	}

	inline VkRect2D& GetScissor()
	{
		return scissor;
	}
};

#endif