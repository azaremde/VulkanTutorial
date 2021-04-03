#ifndef __AstrumVK_UBO_UniformBuffer_hpp__
#define __AstrumVK_UBO_UniformBuffer_hpp__

#pragma once

#include "Pch.hpp"

#include "AstrumVK/GPU/GPU.hpp"
#include "AstrumVK/SwapChain/SwapChain.hpp"
#include "AstrumVK/Pipeline/Pipeline.hpp"

#include "UniformBufferObject.hpp"

#include "AstrumVK/Models/Entity.hpp"

struct UniformLayout
{
    VkDescriptorType type; 

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    
    uint32_t size { 0 };
    uint32_t binding { 0 };
    uint32_t instances { 0 };   // UboIsDynamic = instances > 0

    uint32_t dynamicAlignment { 0 };
    uint32_t bufferSize { 0 };

    bool dynamic { false };     // Reserved for future use.

    std::vector<VkImageView> imageViews;
    std::vector<VkSampler> samplers;

    VkImageView imageView_0;
    VkSampler sampler_0;

    VkImageView imageView_1;
    VkSampler sampler_1;
};

class UniformBuffer
{
private:
    GPU& gpu;
    SwapChain& swapChain;
    Pipeline& pipeline;

    void createUniformBuffers();
    void destroyUniformBuffers();

    VkDescriptorPool descriptorPool;
    void createDescriptorPool();
    void destroyDescriptorPool();

    void allocateDescriptorSets_0();
    void allocateDescriptorSets_1();

    void allocateDescriptorSets_n(int n);

    uint32_t amountOfEntities { 0 };
    std::vector<Entity*> vaos;

    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer& operator=(const UniformBuffer&) = delete;

public:
    std::vector<VkDescriptorSet> descriptorSets_0;
    std::vector<VkDescriptorSet> descriptorSets_1;

    std::vector<UniformLayout> layouts;

    const std::vector<VkDescriptorSet>& getDescriptorSets() const;
    
    void updateUniformBuffer(uint32_t imageIndex, uint32_t index, uint32_t size, void* data);

    UniformBuffer(
        GPU& _gpu, 
        SwapChain& _swapChain, 
        Pipeline& _pipeline, 
        std::vector<UniformLayout> _layouts
    );

    UniformBuffer(
        GPU& _gpu, 
        SwapChain& _swapChain, 
        Pipeline& _pipeline, 
        std::vector<UniformLayout> _layouts,
        std::vector<Entity*>& _vaos
    );

    ~UniformBuffer();
};

#endif