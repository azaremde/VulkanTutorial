#include "UniformBuffer.hpp"

// #include "AstrumVK/GPU/CommandBuffer.hpp"

void UniformBuffer::createDescriptorPool()
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

void UniformBuffer::destroyDescriptorSets()
{
}

void UniformBuffer::createUniformBuffers()
{   
    size_t deviceAlignment = gpu.limits.minUniformBufferOffsetAlignment;
    size_t uniformBufferSize = sizeof(UniformBufferObject);    
    
    dynamicAlignment = (uniformBufferSize / deviceAlignment) * deviceAlignment + ((uniformBufferSize % deviceAlignment) > 0 ? deviceAlignment : 0);
    bSize = uniformBufferSize * instances * dynamicAlignment;

    uniformBuffers.resize(swapChain.getImageCount());
    uniformBuffersMemory.resize(swapChain.getImageCount());

    for (size_t i = 0; i < swapChain.getImageCount(); i++) {
        gpu.createBuffer(
            bSize, 
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            uniformBuffers[i], 
            uniformBuffersMemory[i]
        );
    }    
}

void UniformBuffer::destroyUniformBuffers()
{
    for (size_t i = 0; i < uniformBuffers.size(); i++)
    {        
        vkDestroyBuffer(gpu.getDevice(), uniformBuffers[i], nullptr);
        vkFreeMemory(gpu.getDevice(), uniformBuffersMemory[i], nullptr);
    }
}

void UniformBuffer::updateUniformBuffer(uint32_t imageIndex, UniformBufferObject* ubo)
{
    uint32_t size = dynamicAlignment * instances;

    void* data;
    vkMapMemory(gpu.getDevice(), uniformBuffersMemory[imageIndex], 0, size, 0, &data);
        memcpy(data, ubo, size);
    vkUnmapMemory(gpu.getDevice(), uniformBuffersMemory[imageIndex]);
}

UniformBuffer::UniformBuffer(GPU& _gpu, SwapChain& _swapChain, Pipeline& _pipeline, uint32_t _instances) : gpu { _gpu }, swapChain { _swapChain }, pipeline { _pipeline }, instances { _instances }
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