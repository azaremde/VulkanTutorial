#include "CommandBuffer.hpp"

void CommandBuffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}

VkCommandBuffer CommandBuffer::beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(gpu.getDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void CommandBuffer::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(gpu.getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(gpu.getGraphicsQueue());

    vkFreeCommandBuffers(gpu.getDevice(), commandPool, 1, &commandBuffer);
}

void CommandBuffer::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{    
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    barrier.srcAccessMask = 0; // TODO
    barrier.dstAccessMask = 0; // TODO

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    endSingleTimeCommands(commandBuffer);
}

void CommandBuffer::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{    
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        width,
        height,
        1
    };
    
    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    endSingleTimeCommands(commandBuffer);
}

void CommandBuffer::createVertexBuffer(VAO* vao, const std::vector<Vertex>& vertices)
{
    vao->vertexCount = static_cast<uint32_t>(vertices.size());
    
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    gpu.createBuffer(
        bufferSize, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        stagingBuffer, 
        stagingBufferMemory
    );

    void* data;
    vkMapMemory(gpu.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(gpu.getDevice(), stagingBufferMemory);

    gpu.createBuffer(
        bufferSize, 
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        vao->buffer, 
        vao->memory
    );

    copyBuffer(stagingBuffer, vao->buffer, bufferSize);
    
    vkDestroyBuffer(gpu.getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(gpu.getDevice(), stagingBufferMemory, nullptr);
}

void CommandBuffer::createVertexBuffer(VAO* vao, const std::vector<Vert>& vertices)
{
    vao->vertexCount = static_cast<uint32_t>(vertices.size());
    
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    gpu.createBuffer(
        bufferSize, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        stagingBuffer, 
        stagingBufferMemory
    );

    void* data;
    vkMapMemory(gpu.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(gpu.getDevice(), stagingBufferMemory);

    gpu.createBuffer(
        bufferSize, 
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        vao->buffer, 
        vao->memory
    );

    copyBuffer(stagingBuffer, vao->buffer, bufferSize);
    
    vkDestroyBuffer(gpu.getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(gpu.getDevice(), stagingBufferMemory, nullptr);
}

void CommandBuffer::createIndexBuffer(VAO* vao, const std::vector<uint32_t>& indices)
{
    vao->indexCount = static_cast<uint32_t>(indices.size());

    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    gpu.createBuffer(
        bufferSize, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        stagingBuffer, 
        stagingBufferMemory
    );

    void* data;
    vkMapMemory(gpu.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t) bufferSize);
    vkUnmapMemory(gpu.getDevice(), stagingBufferMemory);

    gpu.createBuffer(
        bufferSize, 
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        vao->indexBuffer, 
        vao->indexBufferMemory
    );

    copyBuffer(stagingBuffer, vao->indexBuffer, bufferSize);

    vkDestroyBuffer(gpu.getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(gpu.getDevice(), stagingBufferMemory, nullptr);
}

void CommandBuffer::createCommandPool()
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = gpu.getQueueFamilyIndices().graphics.value();

    // First option: we want to record it dynamically.
    // Second option: we want to record it once.
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    // poolInfo.flags = 0; // Optional

    VK_CHECK(
        vkCreateCommandPool(gpu.getDevice(), &poolInfo, nullptr, &commandPool),
        "Failed to create command pool."
    );

    DebugLogOut("Command pool created.");
}

void CommandBuffer::destroyCommandPool()
{
    vkDestroyCommandPool(gpu.getDevice(), commandPool, nullptr);

    DebugLogOut("Command pool destroyed.");
}

void CommandBuffer::createCommandBuffers()
{    
    commandBuffers.resize(swapChain.getFramebuffers().size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    VK_CHECK(
        vkAllocateCommandBuffers(gpu.getDevice(), &allocInfo, commandBuffers.data()),
        "Failed to allocate command buffers."
    );
}

void CommandBuffer::render(
    const VkRenderPass& renderPass, 
    const std::vector<Framebuffer*>& swapChainFramebuffers, 
    const VkExtent2D& extent, 
    const VkPipeline& graphicsPipeline,
    const UniformBuffer& uniformBuffer,
    const std::vector<VAO*>& vaos
)
{

    for (size_t i = 0; i < commandBuffers.size(); i++)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        VK_CHECK(
            vkBeginCommandBuffer(commandBuffers[i], &beginInfo),
            "Failed to begin recording command buffer."
        );

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = swapChainFramebuffers[i]->getFramebuffer();

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = extent;

        std::vector<VkClearValue> clearValues(2);
        clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        // VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        VkDeviceSize offsets[] = {0};

        for (size_t j = 0; j < vaos.size(); j++)
        {            
            vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &vaos[j]->buffer, offsets);
            vkCmdBindIndexBuffer(commandBuffers[i], vaos[j]->indexBuffer, 0, VK_INDEX_TYPE_UINT32);

            uint32_t dynamicOffset = static_cast<uint32_t>(j) * uniformBuffer.layouts[0].dynamicAlignment;

            vkCmdBindDescriptorSets(
                commandBuffers[i], 
                VK_PIPELINE_BIND_POINT_GRAPHICS, 
                pipeline.getPipelineLayout(), 
                0, 
                1, 
                &(j == 0 ? uniformBuffer.descriptorSets_0[i] : uniformBuffer.descriptorSets_1[i]),
                1, 
                &dynamicOffset
            );
            
            vkCmdDrawIndexed(commandBuffers[i], vaos[j]->indexCount, 1, 0, 0, 0);
        }

        vkCmdEndRenderPass(commandBuffers[i]);
        VK_CHECK(
            vkEndCommandBuffer(commandBuffers[i]),
            "Failed to record command buffer."
        );
    }
}

void CommandBuffer::freeCommandBuffers()
{
    vkFreeCommandBuffers(
        gpu.getDevice(), 
        commandPool, 
        static_cast<uint32_t>(commandBuffers.size()), 
        commandBuffers.data()
    );
}

CommandBuffer::CommandBuffer(GPU& _gpu, SwapChain& _swapChain, Pipeline& _pipeline) : gpu { _gpu }, swapChain { _swapChain }, pipeline { _pipeline }
{
    createCommandPool();
    createCommandBuffers();
}

CommandBuffer::~CommandBuffer()
{
    destroyCommandPool();
}

const VkCommandPool& CommandBuffer::getCommandPool() const
{
    return commandPool;
}

const std::vector<VkCommandBuffer>& CommandBuffer::getCommandBuffers() const
{
    return commandBuffers;
}