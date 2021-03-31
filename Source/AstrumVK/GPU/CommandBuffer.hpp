#ifndef __AstrumVK_GPU_CommandBuffer_hpp__
#define __AstrumVK_GPU_CommandBuffer_hpp__

#pragma once

#include "Pch.hpp"

#include "GPU.hpp"
#include "AstrumVK/SwapChain/SwapChain.hpp"
#include "AstrumVK/Models/Vertex.hpp"

struct VAO
{
    VkBuffer buffer;
    VkDeviceMemory memory;

    uint32_t vertexCount;

    void destroy(GPU& gpu)
    {
        vkDestroyBuffer(gpu.getDevice(), buffer, nullptr);
        vkFreeMemory(gpu.getDevice(), memory, nullptr);
    }
};

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

public:
    VAO* createVertexBuffer(const std::vector<Vertex>& vertices);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

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