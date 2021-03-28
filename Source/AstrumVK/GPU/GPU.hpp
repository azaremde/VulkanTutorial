#ifndef __AstrumVK_GPU_GPU_hpp__
#define __AstrumVK_GPU_GPU_hpp__

#pragma once

#include "Pch.hpp"

#include "QueueFamilyIndices.hpp"

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
    
    bool isDeviceSuitable(VkPhysicalDevice physicalDevice);

    VkInstance& instance;

    Surface& surface;

    GPU(const GPU&) = delete;
    GPU& operator=(const GPU&) = delete;

public:
    GPU(VkInstance& _instance, Surface& _surface);
    ~GPU();

    const VkPhysicalDevice& getPhysicalDevice() const;
    const VkDevice& getDevice() const;
};

#endif