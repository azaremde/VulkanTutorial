#ifndef __Vulkan_Pipeline_Pipeline_hpp__
#define __Vulkan_Pipeline_Pipeline_hpp__

#pragma once

#include "Pch.hpp"

#include "Vulkan/Shaders/Shader.hpp"
#include "Vulkan/GPU/GPU.hpp"
#include "Vulkan/SwapChain.hpp"
#include "Vulkan/Framebuffers/Framebuffer.hpp"

#include "RenderPass.hpp"

class Pipeline
{
private:
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
	RenderPass* renderPass;

	GPU& gpu;

	SwapChain& swapChain;

	// Temp
	Shader* shader;

	// Temp
	Framebuffer* outputFramebuffer;


	Pipeline(const Pipeline&) = delete;
	Pipeline& operator=(const Pipeline&) = delete;

public:
	Pipeline(GPU& _gpu, SwapChain& _swapChain);
	~Pipeline();

	const RenderPass& GetRenderPass() const;
	const VkPipeline& GetPipeline() const;

	// MAKE IT CONST
	Framebuffer& GetOutputFramebuffer();
};

#endif