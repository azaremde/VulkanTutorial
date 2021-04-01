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
    VkPhysicalDevice physicalDevice { VK_NULL_HANDLE };
    void pickPhysicalDevice();

    VkDevice device { VK_NULL_HANDLE };
    void createLogicalDevice();
    void destroyLogicalDevice();

    struct {
        QueueFamilyIndices familyIndices;
        VkQueue graphics;
        VkQueue present;
    } queues;

    void retrieveQueues();

    SwapChainSupportDetails swapChainSupport;
    
    bool isDeviceSuitable(const VkPhysicalDevice& physicalDevice);

    VkInstance& instance;

    Surface& surface;

    inline static const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    static bool checkDeviceExtensionsSupport(const VkPhysicalDevice& physicalDevice);

    GPU(const GPU&) = delete;
    GPU& operator=(const GPU&) = delete;

public:
    GPU(VkInstance& _instance, Surface& _surface);
    ~GPU();

    void recheckDeviceCapabilities();
    
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    const VkQueue& getGraphicsQueue() const;
    const VkQueue& getPresentQueue() const;

    const VkPhysicalDevice& getPhysicalDevice() const;
    const VkDevice& getDevice() const;
    const QueueFamilyIndices& getQueueFamilyIndices() const;
    const SwapChainSupportDetails& getSwapChainSupportDetails() const;    

    struct {
        VkDeviceSize minUniformBufferOffsetAlignment;
    } limits;
};

#endif