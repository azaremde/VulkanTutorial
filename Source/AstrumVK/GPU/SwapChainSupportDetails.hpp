#ifndef __AstrumVK_GPU_SwapChainSupportDetails_hpp__
#define __AstrumVK_GPU_SwapChainSupportDetails_hpp__

#pragma once

#include "Pch.hpp"

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;

    bool isSupported() const;

    static SwapChainSupportDetails of(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);
};

#endif