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
    void createSwapChain();
    void destroySwapChain();

    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    void createImageViews();
    void destroyImageViews();

    std::vector<Framebuffer*> framebuffers;

    VkSurfaceFormatKHR surfaceFormat;
    void chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    VkPresentModeKHR presentMode;
    void choosePresentationMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

    VkExtent2D extent;
    void chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    /**
     * Finally found this error.
     * The labtop needs for some reason 
     * MAX_FRAMES_IN_FLIGHT = 3;
     * 
     * Todo: CHECK WHETHER THE DEVICE NEEDS 2 OR 3.
     * Todo: substitute to a separate object.
     */
    const int MAX_FRAMES_IN_FLIGHT = 2;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkSemaphore> signalSemaphores;
    uint32_t currentFrame = 0;

    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;

    uint32_t imageIndex;

    GPU& gpu;
    Surface& surface;
    Window& window;

    SwapChain(const SwapChain&) = delete;
    SwapChain& operator=(const SwapChain&) = delete;

public:
    const std::vector<Framebuffer*>& getFramebuffers() const;
    void createFramebuffers(const VkRenderPass& renderPass);
    void destroyFramebuffers();

    void createSemaphores();
    void destroySemaphores();

    uint32_t getImageIndex() const;
    void acquireImage();
    void submit(const std::vector<VkCommandBuffer>& commandBuffers);
    void present();
    
    const VkSurfaceFormatKHR& getSurfaceFormat() const;
    const VkExtent2D& getExtent() const;

    SwapChain(GPU& _gpu, Surface& _surface, Window& _window);
    ~SwapChain();
};

#endif