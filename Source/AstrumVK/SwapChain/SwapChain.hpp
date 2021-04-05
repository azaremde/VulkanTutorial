#ifndef __AstrumVK_SwapChain_SwapChain_hpp__
#define __AstrumVK_SwapChain_SwapChain_hpp__

#pragma once

#include "Pch.hpp"

#include "AstrumVK/GPU/GPU.hpp"
#include "AstrumVK/Surface/Surface.hpp"
#include "AstrumVK/Framebuffers/Framebuffer.hpp"

#include "Core/Window.hpp"

class SwapChain
{
private:
    VkSwapchainKHR swapChain;
    void createSwapChainObject();
    void destroySwapChainObject();

    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    void createImageViews();
    void destroyImageViews();

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;
    void createDepthResources();
    void destroyDepthResources();

    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);    
    bool hasStencilComponent(VkFormat format);

    std::vector<Framebuffer*> framebuffers;

    VkSurfaceFormatKHR surfaceFormat;
    void chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    VkPresentModeKHR presentMode;
    void choosePresentationMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

    VkExtent2D extent;
    void chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    uint32_t imageIndex { 0 };

    bool limitFps { false };

    /**
     * Finally found this error.
     * The labtop needs for some reason 
     * MAX_FRAMES_IN_FLIGHT = 3;
     * 
     * Todo: CHECK WHETHER THE DEVICE NEEDS 2 OR 3.
     */
    struct Sync 
    {
        const int MAX_FRAMES_IN_FLIGHT { 2 };

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkSemaphore> signalSemaphores;

        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;

        void createSyncObjects(uint32_t imageCount);
        void destroySyncObjects();

        uint32_t currentFrame { 0 };
    } sync;

    Window& window;

    SwapChain(const SwapChain&) = delete;
    SwapChain& operator=(const SwapChain&) = delete;

public:
    SwapChain(Window& _window, bool _limitFps = false);
    ~SwapChain();
    
    VkFormat findDepthFormat();

    void createSwapChain();
    void destroySwapChain();

    const std::vector<Framebuffer*>& getFramebuffers() const;
    void createFramebuffers(const VkRenderPass& renderPass);
    void destroyFramebuffers();

    void acquireImage();
    void submit(const std::vector<VkCommandBuffer>& commandBuffers);
    void present();
    
    const VkSurfaceFormatKHR& getSurfaceFormat() const;
    const VkExtent2D& getExtent() const;
    uint32_t getImageIndex() const;
    uint32_t getImageCount() const;
};

#endif