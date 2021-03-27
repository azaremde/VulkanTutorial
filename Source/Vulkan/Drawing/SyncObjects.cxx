#include "SyncObjects.hpp"

SyncObjects::SyncObjects(GPU& _gpu, SwapChain& _swapChain) : gpu { _gpu }, swapChain { _swapChain }
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	imagesInFlight.resize(swapChain.GetSwapChainImages().size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(gpu.Device(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(gpu.Device(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(gpu.Device(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
		{

			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}

	signals.resize(signalSemaphoreCount);
}

SyncObjects::~SyncObjects()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(gpu.Device(), renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(gpu.Device(), imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(gpu.Device(), inFlightFences[i], nullptr);
	}
}

void SyncObjects::PrepareFences()
{
	vkWaitForFences(gpu.Device(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
	vkResetFences(gpu.Device(), 1, &inFlightFences[currentFrame]);
}

void SyncObjects::AcquireImage(uint32_t& imageIndex)
{
	vkAcquireNextImageKHR(gpu.Device(), swapChain.GetSwapChain(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
}

void SyncObjects::RetrieveAndSetNextImage(uint32_t& imageIndex)
{
	// Check if a previous frame is using this image (i.e. there is its fence to wait on)
	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
		vkWaitForFences(gpu.Device(), 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}
	// Mark the image as now being in use by this frame
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];
}

void SyncObjects::SubmitWork(const VkCommandBuffer& buffer)
{
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &buffer;

	signals[0] = renderFinishedSemaphores[currentFrame];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signals.data();

	vkResetFences(gpu.Device(), 1, &inFlightFences[currentFrame]);

	VulkanCheck(
		vkQueueSubmit(gpu.GetQueues().graphics, 1, &submitInfo, inFlightFences[currentFrame]),
		"Failed to submit draw command buffer."
	);
}

void SyncObjects::Present(uint32_t& imageIndex)
{
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signals.data();

	VkSwapchainKHR swapChains[] = { swapChain.GetSwapChain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional

	vkQueuePresentKHR(gpu.GetQueues().present, &presentInfo);

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}