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

    SwapChain& swapChain;

    RenderPass(const RenderPass&) = delete;
    RenderPass& operator=(const RenderPass&) = delete;

public:
    RenderPass(SwapChain& _swapChain);
    ~RenderPass();

    const VkRenderPass& getRenderPass() const;
};

#endif