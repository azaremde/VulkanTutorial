#ifndef __Vulkan_Drawing_hpp__
#define __Vulkan_Drawing_hpp__

#pragma once

#include "Pch.hpp"

#include "Vulkan/GPU/GPU.hpp"
#include "Vulkan/Framebuffers/Framebuffer.hpp"
#include "Vulkan/Pipeline/Pipeline.hpp"
#include "Vulkan/SwapChain.hpp"
#include "SyncObjects.hpp"

const std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	{{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}}
}; 

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0
};

class Drawing
{
private:
	VkCommandPool commandPool;
	void CreateCommandPool();
	void DestroyCommandPool();

	std::vector<VkCommandBuffer> commandBuffers;
	void CreateCommandBuffers();

	SyncObjects* sync;
	void CreateSyncObjects();
	void DestroySyncObjects();

	GPU& gpu;
	Framebuffer& framebuffer;
	Pipeline& pipeline;
	SwapChain& swapChain;

	Drawing(const Drawing&) = delete;
	Drawing& operator=(const Drawing&) = delete;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	void createVertexBuffer();
	void createIndexBuffer();
	void destroyVertexBuffer();
	void destroyIndexBuffer();

public:
	void Destroy();

	Drawing(GPU& _gpu, Framebuffer& _framebuffer, Pipeline& _pipeline, SwapChain& _swapChain);
	~Drawing();

	void Draw();
	void WaitForIdle();
};

#endif