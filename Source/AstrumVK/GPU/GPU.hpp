#ifndef __AstrumVK_GPU_GPU_hpp__
#define __AstrumVK_GPU_GPU_hpp__

#pragma once

#include "Pch.hpp"

#include "QueueFamilyIndices.hpp"
#include "SwapChainSupportDetails.hpp"

#include "AstrumVK/Surface/Surface.hpp"

class GPU
{
private:
    inline static VkPhysicalDevice physicalDevice { VK_NULL_HANDLE };
    static void pickPhysicalDevice();

    inline static VkDevice device { VK_NULL_HANDLE };
    static void createLogicalDevice();
    static void destroyLogicalDevice();

    struct {
        inline static QueueFamilyIndices familyIndices;
        inline static VkQueue graphics;
        inline static VkQueue present;
    } inline static queues;

    static void retrieveQueues();

    inline static SwapChainSupportDetails swapChainSupport;
    
    static bool isDeviceSuitable(const VkPhysicalDevice& physicalDevice);

    inline static VkInstance* instance;

    inline static Surface* surface;

    inline static const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    static bool checkDeviceExtensionsSupport(const VkPhysicalDevice& physicalDevice);

    GPU(const GPU&) = delete;
    GPU& operator=(const GPU&) = delete;

    GPU();
    ~GPU();

public:
    inline static void create(VkInstance* _instance, Surface* _surface)
    {
        instance = _instance;
        surface = _surface;

        pickPhysicalDevice();
        createLogicalDevice();
        retrieveQueues();
    }

    inline static void destroy()
    {
        destroyLogicalDevice();

        DebugLogOut("Logical device has been destroyed.");        
    }

    static void recheckDeviceCapabilities();
    
    static void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    static void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

    // Todo: move into a helper class specially for this.
    static VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

    static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    static const VkQueue& getGraphicsQueue();
    static const VkQueue& getPresentQueue();

    static const VkPhysicalDevice& getPhysicalDevice();
    static const VkDevice& getDevice();
    static const QueueFamilyIndices& getQueueFamilyIndices();
    static const SwapChainSupportDetails& getSwapChainSupportDetails();

    inline static VkPhysicalDeviceProperties props;

    struct {
        inline static std::string name;
    } inline static about;
};

#endif