#ifndef __AstrumVK_UBO_UniformBuffer_hpp__
#define __AstrumVK_UBO_UniformBuffer_hpp__

#pragma once

#include "Pch.hpp"

#include "AstrumVK/GPU/GPU.hpp"
#include "AstrumVK/SwapChain/SwapChain.hpp"
#include "AstrumVK/Pipeline/Pipeline.hpp"

#include "UniformBufferObject.hpp"

class UniformBuffer
{
private:
    GPU& gpu;
    SwapChain& swapChain;
    Pipeline& pipeline;

    UniformBufferObject* ubos;

    uint32_t instances;

    uint32_t dynamicAlignment;
    uint32_t bSize;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
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

public:
    const std::vector<VkDescriptorSet>& getDescriptorSets() const;

    uint32_t getDynamicAlignment() const;
    uint32_t getBufferSize() const;

    void updateUniformBuffer(uint32_t imageIndex, UniformBufferObject* ubo);

    UniformBuffer(GPU& _gpu, SwapChain& _swapChain, Pipeline& _pipeline, uint32_t _instances);
    ~UniformBuffer();
};

#endif