#ifndef __Vulkan_Pipeline_Pipeline_hpp__
#define __Vulkan_Pipeline_Pipeline_hpp__

#pragma once

#include "Pch.hpp"

#include "Vulkan/Shaders/Shader.hpp"
#include "Vulkan/GPU/GPU.hpp"
#include "Vulkan/SwapChain.hpp"
#include "Vulkan/Framebuffers/Framebuffer.hpp"

#include "RenderPass.hpp"

#include <glm/glm.hpp>

#include <array>

struct Vertex
{
	glm::vec2 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};

		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
};

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

	void BeginRenderPass(const VkCommandBuffer& commandBuffer, const VkFramebuffer& framebuffer) const;
	void EndRenderPass(const VkCommandBuffer& commandBuffer) const;

	const RenderPass& GetRenderPass() const;
	const VkPipeline& GetPipeline() const;

	// MAKE IT CONST
	Framebuffer& GetOutputFramebuffer();
};

#endif