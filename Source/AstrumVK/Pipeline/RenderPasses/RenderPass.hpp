#ifndef __AstrumVK_Pipeline_RenderPasses_RenderPass_hpp__
#define __AstrumVK_Pipeline_RenderPasses_RenderPass_hpp__

#pragma once

#include "Pch.hpp"

#include "AstrumVK/GPU/GPU.hpp"
#include "AstrumVK/SwapChain/SwapChain.hpp"

class RenderPass
{
private:
    VkRenderPass renderPass;

    GPU& gpu;
    SwapChain& swapChain;

    RenderPass(const RenderPass&) = delete;
    RenderPass& operator=(const RenderPass&) = delete;

public:
    RenderPass(GPU& _gpu, SwapChain& _swapChain);
    ~RenderPass();

    const VkRenderPass& getRenderPass() const;
};

#endif