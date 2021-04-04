#ifndef __AstrumVK_UBO_UniformBufferLayout_hpp__
#define __AstrumVK_UBO_UniformBufferLayout_hpp__

#pragma once

#include "Pch.hpp"

struct UniformLayout
{
    VkDescriptorType type;
    VkShaderStageFlags stageFlags { VK_SHADER_STAGE_ALL_GRAPHICS };

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    
    uint32_t size { 1 };
    uint32_t binding { 0 };
    uint32_t instances { 0 };   // UboIsDynamic = instances > 0

    uint32_t dynamicAlignment { 0 };
    uint32_t bufferSize { 0 };

    std::vector<VkImageView> imageViews;
    std::vector<VkSampler> samplers;
};

#endif