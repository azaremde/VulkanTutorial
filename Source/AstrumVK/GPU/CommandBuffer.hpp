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
#include "AstrumVK/UBO/UniformBuffer.hpp"

#include "Models/Model.hpp"

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

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

public:
    void createVertexBuffer(VAO* vao, const std::vector<Vertex>& vertices);
    void createVertexBuffer(VAO* vao, const std::vector<Vert>& vertices);
    void createIndexBuffer(VAO* vao, const std::vector<uint32_t>& indices);

    void render(
        const VkRenderPass& renderPass, 
        const std::vector<Framebuffer*>& swapChainFramebuffers, 
        const VkExtent2D& extent, 
        const VkPipeline& graphicsPipeline,
        const UniformBuffer& uniformBuffer,
        const std::vector<VAO*>& vaos
    );

    void createCommandBuffers();
    void freeCommandBuffers();

    const VkCommandPool& getCommandPool() const;
    const std::vector<VkCommandBuffer>& getCommandBuffers() const;

    CommandBuffer(GPU& _gpu, SwapChain& _swapChain, Pipeline& _pipeline);
    ~CommandBuffer();
};

#endif