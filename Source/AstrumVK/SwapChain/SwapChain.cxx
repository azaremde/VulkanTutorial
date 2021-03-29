#include "SwapChain.hpp"

void SwapChain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
    for (const auto &availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            surfaceFormat = availableFormat;
        }
    }

    surfaceFormat = availableFormats[0];
}

void SwapChain::choosePresentationMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
    for (const auto &availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            presentMode = availablePresentMode;
        }
    }

    presentMode = VK_PRESENT_MODE_FIFO_KHR;
}

void SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        extent = capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(window.getGlfwWindow(), &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)};

        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        extent = actualExtent;
    }
}

void SwapChain::createSwapChain()
{
    SwapChainSupportDetails swapChainSupport = gpu.getSwapChainSupportDetails();
    chooseSurfaceFormat(swapChainSupport.formats);
    choosePresentationMode(swapChainSupport.presentModes);
    chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface.getSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = gpu.getQueueFamilyIndices();
    uint32_t queueFamilyIndices[] = {indices.graphics.value(), indices.present.value()};

    if (indices.graphics != indices.present)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;     // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VK_CHECK(
        vkCreateSwapchainKHR(gpu.getDevice(), &createInfo, nullptr, &swapChain),
        "Failed to create swap chain.");

    vkGetSwapchainImagesKHR(gpu.getDevice(), swapChain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(gpu.getDevice(), swapChain, &imageCount, images.data());

    DebugLogOut("Swap chain successfully created.");
}

void SwapChain::destroySwapChain()
{
    vkDestroySwapchainKHR(gpu.getDevice(), swapChain, nullptr);
}

void SwapChain::createImageViews()
{
    imageViews.resize(images.size());

    for (size_t i = 0; i < images.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = surfaceFormat.format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        VK_CHECK(
            vkCreateImageView(gpu.getDevice(), &createInfo, nullptr, &imageViews[i]),
            "Failed to create image view."
        );

        DebugLogOut("Image view created.");
    }
}

void SwapChain::destroyImageViews()
{
    for (const auto& imageView : imageViews)
    {
        vkDestroyImageView(gpu.getDevice(), imageView, nullptr);

        DebugLogOut("Image view destroyed.");
    }
}

void SwapChain::createFramebuffers(const VkRenderPass& renderPass)
{
    for (size_t i = 0; i < imageViews.size(); i++)
    {
        framebuffers.emplace_back(
            new Framebuffer(
                gpu,
                extent,
                { imageViews[i] },
                renderPass
            )
        );
    }
}

void SwapChain::destroyFramebuffers()
{
    for (const auto& framebuffer : framebuffers)
    {
        delete framebuffer;
    }
}

void SwapChain::createSemaphores()
{    
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(gpu.getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(gpu.getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS) {

        throw std::runtime_error("failed to create semaphores!");
    }

    signalSemaphores.resize(1);
}

void SwapChain::destroySemaphores()
{
    vkDestroySemaphore(gpu.getDevice(), renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(gpu.getDevice(), imageAvailableSemaphore, nullptr);
}

uint32_t SwapChain::getImageIndex() const
{
    return imageIndex;
}

void SwapChain::acquireImage()
{    
    vkAcquireNextImageKHR(gpu.getDevice(), swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
}

void SwapChain::submit(const std::vector<VkCommandBuffer>& commandBuffers)
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    // VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    signalSemaphores[0] = renderFinishedSemaphore;
    submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
    submitInfo.pSignalSemaphores = signalSemaphores.data();

    if (vkQueueSubmit(gpu.getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }
}

void SwapChain::present()
{
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
    presentInfo.pWaitSemaphores = signalSemaphores.data();

    VkSwapchainKHR swapChains[] = { swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    vkQueuePresentKHR(gpu.getPresentQueue(), &presentInfo);
}

SwapChain::SwapChain(GPU &_gpu, Surface &_surface, Window &_window) : gpu{_gpu}, surface{_surface}, window{_window}
{
    createSwapChain();
    createImageViews();
}

SwapChain::~SwapChain()
{
    destroyImageViews();
    destroySwapChain();

    DebugLogOut("Swap chain destroyed.");
}

const std::vector<Framebuffer*>& SwapChain::getFramebuffers() const
{
    return framebuffers;
}

const VkSurfaceFormatKHR& SwapChain::getSurfaceFormat() const
{
    return surfaceFormat;
}

const VkExtent2D& SwapChain::getExtent() const
{
    return extent;
}