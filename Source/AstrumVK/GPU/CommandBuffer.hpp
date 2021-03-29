#ifndef __AstrumVK_GPU_CommandBuffer_hpp__
#define __AstrumVK_GPU_CommandBuffer_hpp__

#pragma once

#include "Pch.hpp"

#include "GPU.hpp"
#include "AstrumVK/SwapChain/SwapChain.hpp"

class CommandBuffer
{
private:
    VkCommandPool commandPool;
    void createCommandPool();
    void destroyCommandPool();

    std::vector<VkCommandBuffer> commandBuffers;
    void createCommandBuffers();
    void destroyCommandBuffers();

    GPU& gpu;
    SwapChain& swapChain;

    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;

public:
    void begin(const VkRenderPass& renderPass, const std::vector<Framebuffer*>& swapChainFramebuffers, const VkExtent2D& extent, const VkPipeline& graphicsPipeline);
    void end(const VkCommandBuffer& buffer);

    CommandBuffer(GPU& _gpu, SwapChain& _swapChain);
    ~CommandBuffer();
};

#endif