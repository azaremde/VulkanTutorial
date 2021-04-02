#ifndef __AstrumVK_UBO_UniformBuffer_hpp__
#define __AstrumVK_UBO_UniformBuffer_hpp__

#pragma once

#include "Pch.hpp"

#include "AstrumVK/GPU/GPU.hpp"
#include "AstrumVK/SwapChain/SwapChain.hpp"
#include "AstrumVK/Pipeline/Pipeline.hpp"

#include "UniformBufferObject.hpp"

struct UniformLayout
{    
    std::vector<VkBuffer> dynamicUniformBuffers;
    std::vector<VkDeviceMemory> dynamicUniformBuffersMemory;
    
    uint32_t size { 0 };
    uint32_t binding { 0 };
    uint32_t instances { 0 };   // UboIsDynamic = instances > 0

    bool dynamic { false };     // Reserved for future use.
};

class UniformBuffer
{
private:
    GPU& gpu;
    SwapChain& swapChain;
    Pipeline& pipeline;

    DynamicUBO* ubos;

    uint32_t instances;

    uint32_t dynamicAlignment;
    uint32_t bSize;

    std::vector<VkBuffer> dynamicUniformBuffers;
    std::vector<VkDeviceMemory> dynamicUniformBuffersMemory;

    std::vector<VkBuffer> staticUniformBuffers;
    std::vector<VkDeviceMemory> staticUniformBuffersMemory;
    void createUniformBuffers();
    void destroyUniformBuffers();

    VkDescriptorPool descriptorPool;
    void createDescriptorPool();
    void destroyDescriptorPool();

    std::vector<VkDescriptorSet> descriptorSets;
    void createDescriptorSets();
    void destroyDescriptorSets();

    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer& operator=(const UniformBuffer&) = delete;

    std::vector<UniformLayout> layouts;

public:
    const std::vector<VkDescriptorSet>& getDescriptorSets() const;

    uint32_t getDynamicAlignment() const;
    uint32_t getBufferSize() const;

    void updateUniformBuffer(uint32_t imageIndex, DynamicUBO* ubo, StaticUBO& staticUbo);

    UniformBuffer(GPU& _gpu, SwapChain& _swapChain, Pipeline& _pipeline, std::vector<UniformLayout> _layouts, uint32_t _instances);
    ~UniformBuffer();
};

#endif