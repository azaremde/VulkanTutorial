#include "Drawing.hpp"

uint32_t findMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) 
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void copyBuffer(const VkDevice& device, const VkCommandPool& commandPool, const VkQueue& graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) 
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void createBuffer(
	const VkDevice& device,
	const VkPhysicalDevice& physDevice,
	VkDeviceSize size,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkBuffer& buffer,
	VkDeviceMemory& bufferMemory
)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(physDevice, memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void Drawing::createVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(
		gpu.Device(),
		gpu.PhysicalDevice(),
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

	createBuffer(
		gpu.Device(),
		gpu.PhysicalDevice(),
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		vertexBuffer,
		vertexBufferMemory
	);

	copyBuffer(gpu.Device(), commandPool, gpu.GetQueues().graphics, stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(gpu.Device(), stagingBuffer, nullptr);
	vkFreeMemory(gpu.Device(), stagingBufferMemory, nullptr);
}

void Drawing::createIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(
		gpu.Device(),
		gpu.PhysicalDevice(), 
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

	createBuffer(
		gpu.Device(),
		gpu.PhysicalDevice(), 
		bufferSize, 
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
		indexBuffer, 
		indexBufferMemory
	);

	copyBuffer(gpu.Device(), commandPool, gpu.GetQueues().graphics, stagingBuffer, indexBuffer, bufferSize);

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

Drawing::~Drawing()
{
	DestroySyncObjects();
	DestroyCommandPool();

	destroyVertexBuffer();
	destroyIndexBuffer();
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