#ifndef __Vulkan_Drawing_hpp__
#define __Vulkan_Drawing_hpp__

#pragma once

#include "Pch.hpp"

#include "Vulkan/GPU/GPU.hpp"
#include "Vulkan/Framebuffers/Framebuffer.hpp"
#include "Vulkan/Pipeline/Pipeline.hpp"
#include "Vulkan/SwapChain.hpp"
#include "SyncObjects.hpp"

class Drawing
{
private:
	VkCommandPool commandPool;
	void CreateCommandPool();
	void DestroyCommandPool();

	std::vector<VkCommandBuffer> commandBuffers;
	void CreateCommandBuffers();

	void BindDrawingCommands();

	SyncObjects* sync;
	void CreateSyncObjects();
	void DestroySyncObjects();
	//const int MAX_FRAMES_IN_FLIGHT{ 2 };
	//size_t currentFrame{ 0 };
	//std::vector<VkSemaphore> imageAvailableSemaphores;
	//std::vector<VkSemaphore> renderFinishedSemaphores;
	//std::vector<VkFence> inFlightFences;
	//std::vector<VkFence> imagesInFlight;
	//void CreateSemaphores();
	//void DestroySemaphores();

	GPU& gpu;
	Framebuffer& framebuffer;
	Pipeline& pipeline;
	SwapChain& swapChain;

	Drawing(const Drawing&) = delete;
	Drawing& operator=(const Drawing&) = delete;

public:
	Drawing(GPU& _gpu, Framebuffer& _framebuffer, Pipeline& _pipeline, SwapChain& _swapChain);
	~Drawing();

	void Draw();
	void WaitForIdle();
};

#endif