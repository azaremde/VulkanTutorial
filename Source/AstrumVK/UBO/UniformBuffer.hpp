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
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    
    uint32_t size { 0 };
    uint32_t binding { 0 };
    uint32_t instances { 0 };   // UboIsDynamic = instances > 0

    uint32_t dynamicAlignment { 0 };
    uint32_t bufferSize { 0 };

    bool dynamic { false };     // Reserved for future use.
};

class UniformBuffer
{
private:
    GPU& gpu;
    SwapChain& swapChain;
    Pipeline& pipeline;

    // DynamicUBO* ubos;

    // uint32_t instances;

    // uint32_t dynamicAlignment;
    // uint32_t bSize;

    // std::vector<VkBuffer> dynamicUniformBuffers;
    // std::vector<VkDeviceMemory> dynamicUniformBuffersMemory;

    // std::vector<VkBuffer> staticUniformBuffers;
    // std::vector<VkDeviceMemory> staticUniformBuffersMemory;
    // void createUniformBuffers();
    void _createUniformBuffers();
    // void destroyUniformBuffers();
    void _destroyUniformBuffers();

    VkDescriptorPool descriptorPool;
    // void createDescriptorPool();
    void _createDescriptorPool();
    // void destroyDescriptorPool();
    void _destroyDescriptorPool();

    std::vector<VkDescriptorSet> descriptorSets;
    // void createDescriptorSets();
    void _createDescriptorSets();
    // void destroyDescriptorSets();
    void _destroyDescriptorSets();

    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer& operator=(const UniformBuffer&) = delete;

public:

    std::vector<UniformLayout> layouts;
    
    const std::vector<VkDescriptorSet>& getDescriptorSets() const;

    // uint32_t getDynamicAlignment() const;
    // uint32_t getBufferSize() const;

    // void updateUniformBuffer(uint32_t imageIndex, DynamicUBO* ubo, StaticUBO& staticUbo);
    void updateUniformBuffer(uint32_t imageIndex, uint32_t index, uint32_t size, void* data);

    UniformBuffer(
        GPU& _gpu, 
        SwapChain& _swapChain, 
        Pipeline& _pipeline, 
        std::vector<UniformLayout> _layouts
        // , uint32_t _instances
    );
    ~UniformBuffer();
};

#endif