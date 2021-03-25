#include "Drawing.hpp"

Drawing::Drawing(GPU& _gpu, Framebuffer& _framebuffer, Pipeline& _pipeline, SwapChain& _swapChain) : gpu{ _gpu }, framebuffer{ _framebuffer }, pipeline{ _pipeline }, swapChain{ _swapChain }
{
	CreateCommandPool();
	CreateCommandBuffers();
	CreateSyncObjects();
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

void Drawing::CreateSyncObjects()
{
	sync = new SyncObjects(gpu, swapChain);
}

void Drawing::DestroySyncObjects()
{
	delete sync;
}

Drawing::~Drawing()
{
	DestroySyncObjects();
	DestroyCommandPool();
}

void Drawing::Draw()
{
	uint32_t imageIndex;

	sync->PrepareFences();
	sync->RetrieveAndSetNextImage(imageIndex);
	sync->SubmitWork(commandBuffers[imageIndex]);
	sync->Present(imageIndex);
}

void Drawing::WaitForIdle()
{
	vkDeviceWaitIdle(gpu.Device());
}