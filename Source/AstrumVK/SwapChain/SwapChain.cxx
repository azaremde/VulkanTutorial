#include "SwapChain.hpp"

void SwapChain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
    for (const auto &availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            surfaceFormat = availableFormat;
            return;
        }
    }

    surfaceFormat = availableFormats[0];
}

void SwapChain::choosePresentationMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
    if (limitFps)
    {
        presentMode = VK_PRESENT_MODE_FIFO_KHR;
        return;
    }

    for (const auto &availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            presentMode = availablePresentMode;
            return;
        }
    }

    presentMode = VK_PRESENT_MODE_FIFO_KHR;
}

void SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        extent = capabilities.currentExtent;
        return;
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
        return;
    }
}

void SwapChain::createSwapChainObject()
{
    SwapChainSupportDetails swapChainSupport = GPU::getSwapChainSupportDetails();
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
    createInfo.surface = Surface::getSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = GPU::getQueueFamilyIndices();
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
        vkCreateSwapchainKHR(GPU::getDevice(), &createInfo, nullptr, &swapChain),
        "Failed to create swap chain."
    );

    vkGetSwapchainImagesKHR(GPU::getDevice(), swapChain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(GPU::getDevice(), swapChain, &imageCount, images.data());

    DebugLogOut("Swap chain successfully created.");
}

void SwapChain::destroySwapChainObject()
{
    vkDestroySwapchainKHR(GPU::getDevice(), swapChain, nullptr);
}

void SwapChain::createImageViews()
{
    imageViews.resize(images.size());

    for (size_t i = 0; i < images.size(); i++)
    {
        imageViews[i] = GPU::createImageView(images[i], surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);

        DebugLogOut("Image view created.");
    }
}

void SwapChain::destroyImageViews()
{
    for (const auto& imageView : imageViews)
    {
        vkDestroyImageView(GPU::getDevice(), imageView, nullptr);

        DebugLogOut("Image view destroyed.");
    }

    imageViews.clear();
    images.clear();
}

void SwapChain::createFramebuffers(const VkRenderPass& renderPass)
{
    framebuffers.clear();

    for (size_t i = 0; i < imageViews.size(); i++)
    {
        framebuffers.emplace_back(new Framebuffer(extent, { imageViews[i], depthImageView }, renderPass));
    }
}

void SwapChain::destroyFramebuffers()
{
    for (const auto& framebuffer : framebuffers)
    {
        delete framebuffer;
    }
}

void SwapChain::Sync::createSyncObjects(uint32_t imageCount)
{
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight.resize(imageCount, VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(GPU::getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(GPU::getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(GPU::getDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
        {

            throw std::runtime_error("Failed to create semaphores.");
        }
    }

    signalSemaphores.resize(1);
}

void SwapChain::Sync::destroySyncObjects()
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(GPU::getDevice(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(GPU::getDevice(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(GPU::getDevice(), inFlightFences[i], nullptr);
    }
}

void SwapChain::acquireImage()
{    
    vkWaitForFences(GPU::getDevice(), 1, &sync.inFlightFences[sync.currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(GPU::getDevice(), 1, &sync.inFlightFences[sync.currentFrame]);

    vkAcquireNextImageKHR(GPU::getDevice(), swapChain, UINT64_MAX, sync.imageAvailableSemaphores[sync.currentFrame], VK_NULL_HANDLE, &imageIndex);
}

void SwapChain::submit(const std::vector<VkCommandBuffer>& commandBuffers)
{    
    // Check if a previous frame is using this image (i.e. there is its fence to wait on)
    if (sync.imagesInFlight[imageIndex] != VK_NULL_HANDLE) 
    {
        vkWaitForFences(GPU::getDevice(), 1, &sync.imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }

    // Mark the image as now being in use by this frame
    sync.imagesInFlight[imageIndex] = sync.inFlightFences[sync.currentFrame];

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { sync.imageAvailableSemaphores[sync.currentFrame] };
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    sync.signalSemaphores[0] = sync.renderFinishedSemaphores[sync.currentFrame];
    submitInfo.signalSemaphoreCount = static_cast<uint32_t>(sync.signalSemaphores.size());
    submitInfo.pSignalSemaphores = sync.signalSemaphores.data();

    vkResetFences(GPU::getDevice(), 1, &sync.inFlightFences[sync.currentFrame]);

    VK_CHECK(
        vkQueueSubmit(GPU::getGraphicsQueue(), 1, &submitInfo, sync.inFlightFences[sync.currentFrame]), 
        "Failed to submit draw command buffer."
    );
}

void SwapChain::present()
{
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = static_cast<uint32_t>(sync.signalSemaphores.size());
    presentInfo.pWaitSemaphores = sync.signalSemaphores.data();

    VkSwapchainKHR swapChains[] = { swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    vkQueuePresentKHR(GPU::getPresentQueue(), &presentInfo);

    sync.currentFrame = (sync.currentFrame + 1) % sync.MAX_FRAMES_IN_FLIGHT;
}

void SwapChain::createSwapChain()
{
    createSwapChainObject();
    createImageViews();
    sync.createSyncObjects(static_cast<uint32_t>(images.size()));
    createDepthResources();
}

void SwapChain::destroySwapChain()
{
    destroyDepthResources();
    sync.destroySyncObjects();
    destroyImageViews();
    destroySwapChainObject();

    sync.inFlightFences.clear();
    sync.imagesInFlight.clear();

    DebugLogOut("Swap chain destroyed.");
}

VkFormat SwapChain::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    // Todo: check in the GPU class, right after creation of logical device.
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(GPU::getPhysicalDevice(), format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

VkFormat SwapChain::findDepthFormat() 
{
    return findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

bool SwapChain::hasStencilComponent(VkFormat format) 
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void SwapChain::createDepthResources()
{
    VkFormat depthFormat = findDepthFormat();

    GPU::createImage(
        extent.width, 
        extent.height, 
        depthFormat, 
        VK_IMAGE_TILING_OPTIMAL, 
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        depthImage, 
        depthImageMemory);

    depthImageView = GPU::createImageView(
        depthImage, 
        depthFormat, 
        VK_IMAGE_ASPECT_DEPTH_BIT);
}

void SwapChain::destroyDepthResources()
{
    vkDestroyImageView(GPU::getDevice(), depthImageView, nullptr);
    vkDestroyImage(GPU::getDevice(), depthImage, nullptr);
    vkFreeMemory(GPU::getDevice(), depthImageMemory, nullptr);
}

SwapChain::SwapChain(Window &_window, bool _limitFps) : window { _window }, limitFps { _limitFps }
{
    createSwapChain();
}

SwapChain::~SwapChain()
{
    destroySwapChain();
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

uint32_t SwapChain::getImageIndex() const
{
    return imageIndex;
}

uint32_t SwapChain::getImageCount() const
{
    return static_cast<uint32_t>(images.size());
}