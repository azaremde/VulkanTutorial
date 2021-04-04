#ifndef __AstrumVK_UBO_UniformBuffer_hpp__
#define __AstrumVK_UBO_UniformBuffer_hpp__

#pragma once

#include "Pch.hpp"

#include "AstrumVK/GPU/GPU.hpp"
#include "AstrumVK/SwapChain/SwapChain.hpp"
#include "AstrumVK/Pipeline/Pipeline.hpp"
#include "AstrumVK/Entities/Entity.hpp"
#include "UniformBufferLayout.hpp"

class UniformBuffer
{
private:
    SwapChain& swapChain;
    Pipeline& pipeline;

    void createUniformBuffers();
    void destroyUniformBuffers();

    VkDescriptorPool descriptorPool;
    void createDescriptorPool();
    void destroyDescriptorPool();

    void allocateDescriptorSets(uint32_t entityIndex);

    uint32_t amountOfEntities { 0 };
    std::vector<Entity*> entities;

    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer& operator=(const UniformBuffer&) = delete;

public:
    // std::vector<UniformLayout> layouts;
    
    void updateUniformBuffer(uint32_t imageIndex, uint32_t index, uint32_t size, void* data);

    UniformBuffer(
        SwapChain& _swapChain, 
        Pipeline& _pipeline, 
        std::vector<Entity*>& _entities
    );

    ~UniformBuffer();
};

#endif