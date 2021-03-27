#ifndef __Vulkan_Pipeline_RenderPass_hpp__
#define __Vulkan_Pipeline_RenderPass_hpp__

#pragma once

#include "Pch.hpp"

#include "Vulkan/GPU/GPU.hpp"
#include "Vulkan/SwapChain.hpp"

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

	const VkRenderPass& GetRenderPass() const;
};

#endif