#ifndef __AstrumVK_GPU_CommandBuffer_hpp__
#define __AstrumVK_GPU_CommandBuffer_hpp__

#pragma once

#include "Pch.hpp"

#include "GPU.hpp"
#include "AstrumVK/SwapChain/SwapChain.hpp"
#include "AstrumVK/Entities/Entity.hpp"
#include "AstrumVK/Pipeline/Pipeline.hpp"
#include "AstrumVK/UBO/UniformBuffer.hpp"
#include "AstrumVK/Pipeline/InputAssemblyLayouts/Vertex.hpp"

#include "Assets/MeshAsset.hpp"

class CommandBuffer
{
private:
    VkCommandPool commandPool;
    void createCommandPool();
    void destroyCommandPool();

    std::vector<VkCommandBuffer> commandBuffers;

    SwapChain& swapChain;

    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

public:
    CommandBuffer(SwapChain& _swapChain);
    ~CommandBuffer();

    // Todo: Move it somewhere
    void createVertexBuffer(Entity* vao, const std::vector<Vertex>& vertices);
    void createIndexBuffer(Entity* vao, const std::vector<uint32_t>& indices);

    // Todo: Move it somewhere
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    // Todo: Consider a simplification
    void render(
        Pipeline& pipeline,
        const std::vector<Framebuffer*>& swapChainFramebuffers, 
        const VkExtent2D& extent, 
        const std::vector<Entity*>& vaos
    );

    void render(
        uint32_t index,
        Pipeline& pipeline,
        const std::vector<Framebuffer*>& swapChainFramebuffers, 
        const VkExtent2D& extent, 
        const std::vector<Entity*>& vaos
    );

    void createCommandBuffers();
    void freeCommandBuffers();

    const VkCommandPool& getCommandPool() const;
    const std::vector<VkCommandBuffer>& getCommandBuffers() const;
};

#endif