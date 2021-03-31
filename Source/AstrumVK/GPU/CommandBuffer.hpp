#ifndef __AstrumVK_GPU_CommandBuffer_hpp__
#define __AstrumVK_GPU_CommandBuffer_hpp__

#pragma once

#include "Pch.hpp"

#include "GPU.hpp"
#include "AstrumVK/SwapChain/SwapChain.hpp"
#include "AstrumVK/Models/Vertex.hpp"
#include "AstrumVK/Models/VAO.hpp"

class CommandBuffer
{
private:
    VkCommandPool commandPool;
    void createCommandPool();
    void destroyCommandPool();

    std::vector<VkCommandBuffer> commandBuffers;

    GPU& gpu;
    SwapChain& swapChain;

    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

public:
    VAO* createVertexBuffer(const std::vector<Vertex>& vertices);

    void render(
        const VkRenderPass& renderPass, 
        const std::vector<Framebuffer*>& swapChainFramebuffers, 
        const VkExtent2D& extent, 
        const VkPipeline& graphicsPipeline,
        const std::vector<VAO*>& vaos
    );

    void end(const VkCommandBuffer& buffer);

    void createCommandBuffers();
    void freeCommandBuffers();

    const VkCommandPool& getCommandPool() const;
    const std::vector<VkCommandBuffer>& getCommandBuffers() const;

    CommandBuffer(GPU& _gpu, SwapChain& _swapChain);
    ~CommandBuffer();
};

#endif