#include "UniformBuffer.hpp"

void UniformBuffer::createDescriptorPool()
{
    VkDescriptorPoolSize dynamicPoolSize{};
    dynamicPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    dynamicPoolSize.descriptorCount = static_cast<uint32_t>(swapChain.getImageCount());

    VkDescriptorPoolSize staticPoolSize{};
    staticPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    staticPoolSize.descriptorCount = static_cast<uint32_t>(swapChain.getImageCount());

    std::vector<VkDescriptorPoolSize> poolSizes { dynamicPoolSize, staticPoolSize };

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(swapChain.getImageCount());
    
    VK_CHECK(
        vkCreateDescriptorPool(gpu.getDevice(), &poolInfo, nullptr, &descriptorPool), 
        "Failed to create descriptor pool."
    );
}

void UniformBuffer::destroyDescriptorPool()
{    
    vkDestroyDescriptorPool(gpu.getDevice(), descriptorPool, nullptr);
}

void UniformBuffer::createDescriptorSets()
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

    for (size_t i = 0; i < swapChain.getImageCount(); i++) 
    {
        VkDescriptorBufferInfo dynamicBufferInfo{};
        dynamicBufferInfo.buffer = dynamicUniformBuffers[i];
        dynamicBufferInfo.offset = 0;
        dynamicBufferInfo.range = sizeof(DynamicUBO);

        VkDescriptorBufferInfo staticBufferInfo{};
        staticBufferInfo.buffer = staticUniformBuffers[i];
        staticBufferInfo.offset = 0;
        staticBufferInfo.range = sizeof(StaticUBO);

        VkWriteDescriptorSet dynamicDescriptorWrite{};
        dynamicDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        dynamicDescriptorWrite.dstSet = descriptorSets[i];
        dynamicDescriptorWrite.dstBinding = 0;
        dynamicDescriptorWrite.dstArrayElement = 0;
        dynamicDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        dynamicDescriptorWrite.descriptorCount = 1;
        dynamicDescriptorWrite.pBufferInfo = &dynamicBufferInfo;

        VkWriteDescriptorSet staticDescriptorWrite{};
        staticDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        staticDescriptorWrite.dstSet = descriptorSets[i];
        staticDescriptorWrite.dstBinding = 1;
        staticDescriptorWrite.dstArrayElement = 0;
        staticDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        staticDescriptorWrite.descriptorCount = 1;
        staticDescriptorWrite.pBufferInfo = &staticBufferInfo;

        std::vector<VkWriteDescriptorSet> writeDescriptorSets = { dynamicDescriptorWrite, staticDescriptorWrite };

        vkUpdateDescriptorSets(
            gpu.getDevice(), 
            static_cast<uint32_t>(writeDescriptorSets.size()), 
            writeDescriptorSets.data(), 
            0, 
            nullptr
        );
    }
}

void UniformBuffer::destroyDescriptorSets()
{
}

void UniformBuffer::createUniformBuffers()
{   
    uint32_t deviceAlignment = static_cast<uint32_t>(gpu.limits.minUniformBufferOffsetAlignment);
    uint32_t uniformBufferSize = sizeof(DynamicUBO);    
    
    dynamicAlignment = (uniformBufferSize / deviceAlignment) * deviceAlignment + ((uniformBufferSize % deviceAlignment) > 0 ? deviceAlignment : 0);
    bSize = uniformBufferSize * instances * dynamicAlignment;

    dynamicUniformBuffers.resize(swapChain.getImageCount());
    dynamicUniformBuffersMemory.resize(swapChain.getImageCount());

    for (size_t i = 0; i < swapChain.getImageCount(); i++) {
        gpu.createBuffer(
            bSize, 
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            dynamicUniformBuffers[i], 
            dynamicUniformBuffersMemory[i]
        );
    }

    staticUniformBuffers.resize(swapChain.getImageCount());
    staticUniformBuffersMemory.resize(swapChain.getImageCount());

    for (size_t i = 0; i < swapChain.getImageCount(); i++) {
        gpu.createBuffer(
            sizeof(StaticUBO), 
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            staticUniformBuffers[i], 
            staticUniformBuffersMemory[i]
        );
    }
}

void UniformBuffer::destroyUniformBuffers()
{
    for (size_t i = 0; i < dynamicUniformBuffers.size(); i++)
    {        
        vkDestroyBuffer(gpu.getDevice(), dynamicUniformBuffers[i], nullptr);
        vkFreeMemory(gpu.getDevice(), dynamicUniformBuffersMemory[i], nullptr);
        vkDestroyBuffer(gpu.getDevice(), staticUniformBuffers[i], nullptr);
        vkFreeMemory(gpu.getDevice(), staticUniformBuffersMemory[i], nullptr);
    }
}

void UniformBuffer::updateUniformBuffer(uint32_t imageIndex, DynamicUBO* ubo, StaticUBO& staticUbo)
{
    uint32_t size = dynamicAlignment * instances;

    void* data;
    vkMapMemory(gpu.getDevice(), dynamicUniformBuffersMemory[imageIndex], 0, size, 0, &data);
        memcpy(data, ubo, size);
    vkUnmapMemory(gpu.getDevice(), dynamicUniformBuffersMemory[imageIndex]);

    void* staticUboData;
    vkMapMemory(gpu.getDevice(), staticUniformBuffersMemory[imageIndex], 0, sizeof(staticUbo), 0, &staticUboData);
        memcpy(staticUboData, &staticUbo, sizeof(staticUbo));
    vkUnmapMemory(gpu.getDevice(), staticUniformBuffersMemory[imageIndex]);
}

UniformBuffer::UniformBuffer(
    GPU& _gpu, 
    SwapChain& _swapChain, 
    Pipeline& _pipeline, 
    std::vector<UniformLayout> _layouts, 
    uint32_t _instances
) : 
    gpu { _gpu }, 
    swapChain { _swapChain }, 
    pipeline { _pipeline }, 
    layouts { _layouts }, 
    instances { _instances }
{
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
}

UniformBuffer::~UniformBuffer()
{
    destroyDescriptorSets();
    destroyDescriptorPool();
    destroyUniformBuffers();
}

const std::vector<VkDescriptorSet>& UniformBuffer::getDescriptorSets() const
{
    return descriptorSets;
}

uint32_t UniformBuffer::getDynamicAlignment() const
{
    return dynamicAlignment;
}

uint32_t UniformBuffer::getBufferSize() const
{
    return bSize;
}