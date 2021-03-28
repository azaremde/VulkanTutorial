#ifndef __AstrumVK_GPU_QueueFamilyIndices_hpp__
#define __AstrumVK_GPU_QueueFamilyIndices_hpp__

#pragma once

#include "Pch.hpp"

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphics;
    std::optional<uint32_t> present;

    bool isComplete()
    {
        return graphics.has_value() && present.has_value();
    }

    static QueueFamilyIndices of(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);
};

#endif