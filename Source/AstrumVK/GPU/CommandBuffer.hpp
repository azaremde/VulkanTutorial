#ifndef __AstrumVK_GPU_CommandBuffer_hpp__
#define __AstrumVK_GPU_CommandBuffer_hpp__

#pragma once

#include "Pch.hpp"

#include "GPU.hpp"
#include "AstrumVK/SwapChain/SwapChain.hpp"
#include "AstrumVK/Models/Vertex.hpp"
#include "AstrumVK/Models/VAO.hpp"
#include "AstrumVK/Pipeline/Pipeline.hpp"
#include "AstrumVK/UBO/UniformBufferObject.hpp"

class CommandBuffer
{
private:
    VkCommandPool commandPool;
    void createCommandPool();
    void destroyCommandPool();

    std::vector<VkCommandBuffer> commandBuffers;

    GPU& gpu;
    SwapChain& swapChain;
    Pipeline& pipeline;

    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    // Uniform buffers
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
    // Uniform buffers

public:

    uint32_t dynamicAlignment { 0 };
    uint32_t bSize { 0 };

    void createVertexBuffer(VAO* vao, const std::vector<Vertex>& vertices);
    void createIndexBuffer(VAO* vao, const std::vector<uint32_t>& indices);

    void render(
        const VkRenderPass& renderPass, 
        const std::vector<Framebuffer*>& swapChainFramebuffers, 
        const VkExtent2D& extent, 
        const VkPipeline& graphicsPipeline,
        const std::vector<VAO*>& vaos
    );

    void createCommandBuffers();
    void freeCommandBuffers();

    const VkCommandPool& getCommandPool() const;
    const std::vector<VkCommandBuffer>& getCommandBuffers() const;

    void updateUniformBuffer(uint32_t imageIndex, UniformBufferObject* ubo, uint32_t si);

    CommandBuffer(GPU& _gpu, SwapChain& _swapChain, Pipeline& _pipeline);
    ~CommandBuffer();
};

#endif