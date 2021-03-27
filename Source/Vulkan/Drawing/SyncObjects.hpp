#ifndef __Vulkan_Drawing_SyncObjects_hpp__
#define __Vulkan_Drawing_SyncObjects_hpp__

#pragma once

#include "Pch.hpp"

#include "Vulkan/GPU/GPU.hpp"
#include "Vulkan/SwapChain.hpp"

class SyncObjects
{
private:
	const int MAX_FRAMES_IN_FLIGHT{ 2 };
	const int signalSemaphoreCount{ 1 };

	size_t currentFrame{ 0 };
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

	std::vector<VkSemaphore> signals;

	GPU& gpu;
	SwapChain& swapChain;

public:
	SyncObjects(GPU& _gpu, SwapChain& _swapChain);
	~SyncObjects();

	void PrepareFences();
	void AcquireImage(uint32_t& imageIndex);
	void RetrieveAndSetNextImage(uint32_t& imageIndex);
	void SubmitWork(const VkCommandBuffer& buffer);
	void Present(uint32_t& imageIndex);

};

#endif