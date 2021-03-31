#include "CommandBuffer.hpp"

VAO* CommandBuffer::createVertexBuffer(const std::vector<Vertex>& vertices) {
    VAO* result = new VAO();

    result->vertexCount = static_cast<uint32_t>(vertices.size());

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(vertices[0]) * vertices.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(gpu.getDevice(), &bufferInfo, nullptr, &result->buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(gpu.getDevice(), result->buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(gpu.getDevice(), &allocInfo, nullptr, &result->memory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(gpu.getDevice(), result->buffer, result->memory, 0);

    void* data;
    vkMapMemory(gpu.getDevice(), result->memory, 0, bufferInfo.size, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferInfo.size);
    vkUnmapMemory(gpu.getDevice(), result->memory);

    return result;
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
    poolInfo.flags = 0; // Optional

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

        for (const VAO* vao : vaos)
        {            
            vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &vao->buffer, offsets);
            vkCmdDraw(commandBuffers[i], vao->vertexCount, 1, 0, 0);
        }

        end(commandBuffers[i]);
    }
}

void CommandBuffer::end(const VkCommandBuffer& buffer)
{
    vkCmdEndRenderPass(buffer);
    VK_CHECK(
        vkEndCommandBuffer(buffer),
        "Failed to record command buffer."
    );
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

CommandBuffer::CommandBuffer(GPU& _gpu, SwapChain& _swapChain) : gpu { _gpu }, swapChain { _swapChain }
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