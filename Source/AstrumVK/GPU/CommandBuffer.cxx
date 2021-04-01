#include "CommandBuffer.hpp"

void CommandBuffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{    
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(gpu.getDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(gpu.getDevice(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(gpu.getDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(gpu.getDevice(), buffer, bufferMemory, 0);
}

void CommandBuffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
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

    vkQueueSubmit(gpu.getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(gpu.getGraphicsQueue());

    vkFreeCommandBuffers(gpu.getDevice(), commandPool, 1, &commandBuffer);
}

void CommandBuffer::createVertexBuffer(VAO* vao, const std::vector<Vertex>& vertices)
{
    vao->vertexCount = static_cast<uint32_t>(vertices.size());
    
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(
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

    createBuffer(
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
    createBuffer(
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

    createBuffer(
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

uint32_t CommandBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(gpu.getPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
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

void CommandBuffer::createDescriptorPool()
{
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    poolSize.descriptorCount = static_cast<uint32_t>(swapChain.getImageCount());

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(swapChain.getImageCount());
    
    VK_CHECK(
        vkCreateDescriptorPool(gpu.getDevice(), &poolInfo, nullptr, &descriptorPool), 
        "Failed to create descriptor pool."
    );
}

void CommandBuffer::destroyDescriptorPool()
{    
    vkDestroyDescriptorPool(gpu.getDevice(), descriptorPool, nullptr);
}

void CommandBuffer::createDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(swapChain.getImageCount(), pipeline.getDescriptorSetLayout());
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChain.getImageCount());
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(swapChain.getImageCount());
    if (vkAllocateDescriptorSets(gpu.getDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < swapChain.getImageCount(); i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(gpu.getDevice(), 1, &descriptorWrite, 0, nullptr);
    }
}

void CommandBuffer::destroyDescriptorSets()
{
    
}

void CommandBuffer::render(
    const VkRenderPass& renderPass, 
    const std::vector<Framebuffer*>& swapChainFramebuffers, 
    const VkExtent2D& extent, 
    const VkPipeline& graphicsPipeline,
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

        VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        VkDeviceSize offsets[] = {0};

        for (size_t j = 0; j < vaos.size(); j++)
        {            
            vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &vaos[j]->buffer, offsets);
            vkCmdBindIndexBuffer(commandBuffers[i], vaos[j]->indexBuffer, 0, VK_INDEX_TYPE_UINT32);

            uint32_t dynamicOffset = j * static_cast<uint32_t>(dynamicAlignment);

            vkCmdBindDescriptorSets(
                commandBuffers[i], 
                VK_PIPELINE_BIND_POINT_GRAPHICS, 
                pipeline.getPipelineLayout(), 
                0, 
                1, 
                &descriptorSets[i], 
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

void CommandBuffer::createUniformBuffers()
{   
    size_t deviceAlignment = gpu.minUniformBufferOffsetAlignment;
    size_t uniformBufferSize = sizeof(glm::mat4x4);
    dynamicAlignment = (uniformBufferSize / deviceAlignment) * deviceAlignment + ((uniformBufferSize % deviceAlignment) > 0 ? deviceAlignment : 0);

    bSize = uniformBufferSize * 2 * dynamicAlignment;

    // size_t minUboAlignment = gpu.minUniformBufferOffsetAlignment;
    // dynamicAlignment = sizeof(glm::mat4);
    // if (minUboAlignment > 0) {
    //     dynamicAlignment = (dynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);
    // }

    // bSize = 2 * dynamicAlignment;

    uniformBuffers.resize(swapChain.getImageCount());
    uniformBuffersMemory.resize(swapChain.getImageCount());

    for (size_t i = 0; i < swapChain.getImageCount(); i++) {
        createBuffer(
            bSize, 
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            uniformBuffers[i], 
            uniformBuffersMemory[i]
        );
    }    
}

void CommandBuffer::destroyUniformBuffers()
{
    for (size_t i = 0; i < uniformBuffers.size(); i++)
    {        
        vkDestroyBuffer(gpu.getDevice(), uniformBuffers[i], nullptr);
        vkFreeMemory(gpu.getDevice(), uniformBuffersMemory[i], nullptr);
    }
}

void CommandBuffer::updateUniformBuffer(uint32_t imageIndex, UniformBufferObject* ubo, uint32_t size)
{
    void* data;
    vkMapMemory(gpu.getDevice(), uniformBuffersMemory[imageIndex], 0, size, 0, &data);
        memcpy(data, ubo, size);
    vkUnmapMemory(gpu.getDevice(), uniformBuffersMemory[imageIndex]);
}

CommandBuffer::CommandBuffer(GPU& _gpu, SwapChain& _swapChain, Pipeline& _pipeline) : gpu { _gpu }, swapChain { _swapChain }, pipeline { _pipeline }
{
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();

    createCommandPool();
    createCommandBuffers();
}

CommandBuffer::~CommandBuffer()
{
    destroyCommandPool();
    
    destroyDescriptorSets();
    destroyDescriptorPool();
    destroyUniformBuffers();
}

const VkCommandPool& CommandBuffer::getCommandPool() const
{
    return commandPool;
}

const std::vector<VkCommandBuffer>& CommandBuffer::getCommandBuffers() const
{
    return commandBuffers;
}