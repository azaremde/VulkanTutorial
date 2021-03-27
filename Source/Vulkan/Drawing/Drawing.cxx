#include "Drawing.hpp"

void Drawing::createVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	gpu.CreateBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory
	);
	
	void* data;
	vkMapMemory(gpu.Device(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(gpu.Device(), stagingBufferMemory);

	gpu.CreateBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		vertexBuffer,
		vertexBufferMemory
	);

	gpu.CopyBuffer(commandPool, stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(gpu.Device(), stagingBuffer, nullptr);
	vkFreeMemory(gpu.Device(), stagingBufferMemory, nullptr);
}

void Drawing::createIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	gpu.CreateBuffer(
		bufferSize, 
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		stagingBuffer, 
		stagingBufferMemory
	);

	void* data;
	vkMapMemory(gpu.Device(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(gpu.Device(), stagingBufferMemory);

	gpu.CreateBuffer(
		bufferSize, 
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
		indexBuffer, 
		indexBufferMemory
	);

	gpu.CopyBuffer(commandPool, stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(gpu.Device(), stagingBuffer, nullptr);
	vkFreeMemory(gpu.Device(), stagingBufferMemory, nullptr);
}

void Drawing::destroyVertexBuffer()
{
	vkDestroyBuffer(gpu.Device(), vertexBuffer, nullptr);
	vkFreeMemory(gpu.Device(), vertexBufferMemory, nullptr);
}

void Drawing::destroyIndexBuffer()
{
	vkDestroyBuffer(gpu.Device(), indexBuffer, nullptr);
	vkFreeMemory(gpu.Device(), indexBufferMemory, nullptr);
}

Drawing::Drawing(GPU& _gpu, Framebuffer& _framebuffer, Pipeline& _pipeline, SwapChain& _swapChain) : gpu{ _gpu }, framebuffer{ _framebuffer }, pipeline{ _pipeline }, swapChain{ _swapChain }
{
	CreateCommandPool();

	createVertexBuffer();
	createIndexBuffer();

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

		pipeline.BeginRenderPass(commandBuffers[i], framebuffer.GetSwapChainFramebuffers()[i]);

			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetPipeline());

			VkBuffer vertexBuffers[] = { vertexBuffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT16);

			vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.__GetPipelineLayout(), 0, 1, &pipeline.__GetDescriptorSets()[i], 0, nullptr);

			vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

		pipeline.EndRenderPass(commandBuffers[i]);

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

void Drawing::Destroy()
{
	DestroySyncObjects();
	DestroyCommandPool();

	destroyVertexBuffer();
	destroyIndexBuffer();
}

Drawing::~Drawing()
{
	Destroy();
}

void Drawing::UpdateUniformBuffer(uint32_t imageIndex)
{
	static float timer{0};

	timer += 1;

	UniformBufferObject ubo{};

	ubo.model = glm::mat4x4(1);
	ubo.model = glm::rotate(ubo.model, glm::radians(timer), glm::vec3(0, 0, 1));
	ubo.model = glm::translate(ubo.model, glm::vec3(0, 0, -5));

	ubo.view = glm::mat4x4(1);

	ubo.proj = glm::mat4x4(1);
	ubo.proj = glm::perspective(
		glm::radians(70.0f), 
		static_cast<float>(swapChain.GetSwapChainExtent().width) / static_cast<float>(swapChain.GetSwapChainExtent().height), 
		0.1f, 
		1000.0f
	);

	pipeline.UpdateUniformBuffer(imageIndex, ubo);
}

void Drawing::Draw()
{
	static float deltaTime{ 0 };
	static float lastTime{ 0 };
	static float timer{ 0 };
	static float currentTime{ 0 };
	static unsigned int fps{ 0 };

	timer += deltaTime;

	if (timer >= 1)
	{
		fps = static_cast<unsigned int>(1.0f / deltaTime);
		LogOut(fps);

		timer = 0;
	}

	uint32_t imageIndex;

	sync->PrepareFences();

	sync->AcquireImage(imageIndex);

	UpdateUniformBuffer(imageIndex);

	sync->RetrieveAndSetNextImage(imageIndex);


	sync->SubmitWork(commandBuffers[imageIndex]);
	sync->Present(imageIndex);

	currentTime = glfwGetTime();
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;
}

void Drawing::WaitForIdle()
{
	vkDeviceWaitIdle(gpu.Device());
}