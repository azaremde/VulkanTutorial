#ifndef __Vulkan_Framebuffers_Framebuffer_hpp__
#define __Vulkan_Framebuffers_Framebuffer_hpp__

#pragma once

#include "Pch.hpp"

#include "Vulkan/GPU/GPU.hpp"
#include "Vulkan/SwapChain.hpp"
#include "Vulkan/Pipeline/RenderPass.hpp"

class Framebuffer
{
private:
	std::vector<VkFramebuffer> swapChainFramebuffers;

	GPU& gpu;
	SwapChain& swapChain;
	RenderPass& renderPass;

	Framebuffer(const Framebuffer&) = delete;
	Framebuffer& operator=(const Framebuffer&) = delete;

public:
	Framebuffer(GPU& _gpu, SwapChain& _swapChain, RenderPass& _renderPass);
	~Framebuffer();

	const std::vector<VkFramebuffer>& GetSwapChainFramebuffers() const;
};

#endif