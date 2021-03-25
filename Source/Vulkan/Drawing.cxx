#include "Drawing.hpp"

Drawing::Drawing(GPU& _gpu, Framebuffer& _framebuffer, Pipeline& _pipeline, SwapChain& _swapChain) : gpu{ _gpu }, framebuffer{ _framebuffer }, pipeline{ _pipeline }, swapChain{ _swapChain }
{
	CreateCommandPool();
	CreateCommandBuffers();
	CreateSemaphores();
}

void Drawing::CreateCommandPool()
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = gpu.GetQueues().familyIndices.graphicsFamily.value();
	poolInfo.flags = 0;

	VulkanCheck(
		vkCreateCommandPool(gpu.Device(), &poolInfo, nullptr, &commandPool),
		"Failed to create command pool."
	);
}

void Drawing::DestroyCommandPool()
{
	vkDestroyCommandPool(gpu.Device(), commandPool, nullptr);
}

void Drawing::CreateCommandBuffers()
{
	commandBuffers.resize(framebuffer.GetSwapChainFramebuffers().size());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	VulkanCheck(
		vkAllocateCommandBuffers(gpu.Device(), &allocInfo, commandBuffers.data()),
		"Failed to allocate command buffers."
	);

	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Opt
		beginInfo.pInheritanceInfo = nullptr; // Opt

		VulkanCheck(
			vkBeginCommandBuffer(commandBuffers[i], &beginInfo),
			"Failed to begin recording command buffer."
		);

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = pipeline.GetRenderPass().GetRenderPass();
		renderPassInfo.framebuffer = framebuffer.GetSwapChainFramebuffers()[i];

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChain.GetSwapChainExtent();

		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetPipeline());

		vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		VulkanCheck(
			vkEndCommandBuffer(commandBuffers[i]),
			"Failed to record command buffer."
		);
	}

	// Think about this..
	//BindDrawingCommands();
}

void Drawing::BindDrawingCommands()
{
	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetPipeline());

		vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		VulkanCheck(
			vkEndCommandBuffer(commandBuffers[i]),
			"Failed to record command buffer."
		);
	}
}

void Drawing::CreateSemaphores()
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
}

void Drawing::DestroySemaphores()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(gpu.Device(), renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(gpu.Device(), imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(gpu.Device(), inFlightFences[i], nullptr);
	}
}

Drawing::~Drawing()
{
	DestroySemaphores();
	DestroyCommandPool();
}

void Drawing::Draw()
{
	vkWaitForFences(gpu.Device(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
	vkResetFences(gpu.Device(), 1, &inFlightFences[currentFrame]);

	uint32_t imageIndex;

	vkAcquireNextImageKHR(gpu.Device(), swapChain.GetSwapChain(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	// Check if a previous frame is using this image (i.e. there is its fence to wait on)
	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
		vkWaitForFences(gpu.Device(), 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}
	// Mark the image as now being in use by this frame
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(gpu.Device(), 1, &inFlightFences[currentFrame]);

	VulkanCheck(
		vkQueueSubmit(gpu.GetQueues().graphics, 1, &submitInfo, inFlightFences[currentFrame]),
		"Failed to submit draw command buffer."
	);

	// Getting result from the swap chain:
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain.GetSwapChain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional

	vkQueuePresentKHR(gpu.GetQueues().present, &presentInfo);

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Drawing::WaitForIdle()
{
	vkDeviceWaitIdle(gpu.Device());
}