#include "UniformBuffer.hpp"

// void UniformBuffer::createDescriptorPool()
// {
//     VkDescriptorPoolSize dynamicPoolSize{};
//     dynamicPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
//     dynamicPoolSize.descriptorCount = static_cast<uint32_t>(swapChain.getImageCount());

//     VkDescriptorPoolSize staticPoolSize{};
//     staticPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//     staticPoolSize.descriptorCount = static_cast<uint32_t>(swapChain.getImageCount());

//     std::vector<VkDescriptorPoolSize> poolSizes { dynamicPoolSize, staticPoolSize };

//     VkDescriptorPoolCreateInfo poolInfo{};
//     poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//     poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
//     poolInfo.pPoolSizes = poolSizes.data();
//     poolInfo.maxSets = static_cast<uint32_t>(swapChain.getImageCount());
    
//     VK_CHECK(
//         vkCreateDescriptorPool(gpu.getDevice(), &poolInfo, nullptr, &descriptorPool), 
//         "Failed to create descriptor pool."
//     );
// }

void UniformBuffer::_createDescriptorPool()
{
    std::vector<VkDescriptorPoolSize> poolSizes;

    for (size_t i = 0; i < layouts.size(); i++)
    {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = layouts[i].instances > 0 ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = static_cast<uint32_t>(swapChain.getImageCount());

        poolSizes.emplace_back(poolSize);
    }

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

// void UniformBuffer::destroyDescriptorPool()
// {    
//     vkDestroyDescriptorPool(gpu.getDevice(), descriptorPool, nullptr);
// }

void UniformBuffer::_destroyDescriptorPool()
{    
    vkDestroyDescriptorPool(gpu.getDevice(), descriptorPool, nullptr);
}

// void UniformBuffer::createDescriptorSets()
// {
//     std::vector<VkDescriptorSetLayout> layouts(swapChain.getImageCount(), pipeline.getDescriptorSetLayout());
//     VkDescriptorSetAllocateInfo allocInfo{};
//     allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//     allocInfo.descriptorPool = descriptorPool;
//     allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChain.getImageCount());
//     allocInfo.pSetLayouts = layouts.data();

//     descriptorSets.resize(swapChain.getImageCount());
//     if (vkAllocateDescriptorSets(gpu.getDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
//         throw std::runtime_error("failed to allocate descriptor sets!");
//     }

//     for (size_t i = 0; i < swapChain.getImageCount(); i++) 
//     {
//         VkDescriptorBufferInfo dynamicBufferInfo{};
//         dynamicBufferInfo.buffer = dynamicUniformBuffers[i];
//         dynamicBufferInfo.offset = 0;
//         dynamicBufferInfo.range = sizeof(DynamicUBO);

//         VkDescriptorBufferInfo staticBufferInfo{};
//         staticBufferInfo.buffer = staticUniformBuffers[i];
//         staticBufferInfo.offset = 0;
//         staticBufferInfo.range = sizeof(StaticUBO);

//         VkWriteDescriptorSet dynamicDescriptorWrite{};
//         dynamicDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//         dynamicDescriptorWrite.dstSet = descriptorSets[i];
//         dynamicDescriptorWrite.dstBinding = 0;
//         dynamicDescriptorWrite.dstArrayElement = 0;
//         dynamicDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
//         dynamicDescriptorWrite.descriptorCount = 1;
//         dynamicDescriptorWrite.pBufferInfo = &dynamicBufferInfo;

//         VkWriteDescriptorSet staticDescriptorWrite{};
//         staticDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//         staticDescriptorWrite.dstSet = descriptorSets[i];
//         staticDescriptorWrite.dstBinding = 1;
//         staticDescriptorWrite.dstArrayElement = 0;
//         staticDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//         staticDescriptorWrite.descriptorCount = 1;
//         staticDescriptorWrite.pBufferInfo = &staticBufferInfo;

//         std::vector<VkWriteDescriptorSet> writeDescriptorSets = { dynamicDescriptorWrite, staticDescriptorWrite };

//         vkUpdateDescriptorSets(
//             gpu.getDevice(), 
//             static_cast<uint32_t>(writeDescriptorSets.size()), 
//             writeDescriptorSets.data(), 
//             0, 
//             nullptr
//         );
//     }
// }

void UniformBuffer::_createDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> ls(swapChain.getImageCount(), pipeline.getDescriptorSetLayout());
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChain.getImageCount());
    allocInfo.pSetLayouts = ls.data();

    descriptorSets.resize(swapChain.getImageCount());
    if (vkAllocateDescriptorSets(gpu.getDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < swapChain.getImageCount(); i++) 
    {
        std::vector<VkWriteDescriptorSet> writeDescriptorSets;
        std::vector<VkDescriptorBufferInfo> bufferInfos;
        bufferInfos.resize(layouts.size());

        for (size_t j = 0; j < layouts.size(); j++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = layouts[j].uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = layouts[j].size;
            bufferInfos[j] = bufferInfo;

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSets[i];
            descriptorWrite.dstBinding = layouts[j].binding;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = layouts[j].instances > 0 ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfos[j];

            writeDescriptorSets.emplace_back(descriptorWrite);
        }

        vkUpdateDescriptorSets(
            gpu.getDevice(), 
            static_cast<uint32_t>(writeDescriptorSets.size()), 
            writeDescriptorSets.data(), 
            0, 
            nullptr
        );
    }
}

// void UniformBuffer::destroyDescriptorSets()
// {
// }

void UniformBuffer::_destroyDescriptorSets()
{
}

// void UniformBuffer::createUniformBuffers()
// {   
//     uint32_t deviceAlignment = static_cast<uint32_t>(gpu.limits.minUniformBufferOffsetAlignment);
//     uint32_t uniformBufferSize = sizeof(DynamicUBO);    
    
//     dynamicAlignment = (uniformBufferSize / deviceAlignment) * deviceAlignment + ((uniformBufferSize % deviceAlignment) > 0 ? deviceAlignment : 0);
//     bSize = uniformBufferSize * instances * dynamicAlignment;

//     dynamicUniformBuffers.resize(swapChain.getImageCount());
//     dynamicUniformBuffersMemory.resize(swapChain.getImageCount());

//     for (size_t i = 0; i < swapChain.getImageCount(); i++) {
//         gpu.createBuffer(
//             bSize, 
//             VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
//             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
//             dynamicUniformBuffers[i], 
//             dynamicUniformBuffersMemory[i]
//         );
//     }

//     staticUniformBuffers.resize(swapChain.getImageCount());
//     staticUniformBuffersMemory.resize(swapChain.getImageCount());

//     for (size_t i = 0; i < swapChain.getImageCount(); i++) {
//         gpu.createBuffer(
//             sizeof(StaticUBO), 
//             VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
//             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
//             staticUniformBuffers[i], 
//             staticUniformBuffersMemory[i]
//         );
//     }
// }

void UniformBuffer::_createUniformBuffers()
{
    for (size_t i = 0; i < layouts.size(); i++)
    {
        uint32_t deviceAlignment = static_cast<uint32_t>(gpu.limits.minUniformBufferOffsetAlignment);
        uint32_t uniformBufferSize = layouts[i].size;    
        
        layouts[i].dynamicAlignment = (uniformBufferSize / deviceAlignment) * deviceAlignment + ((uniformBufferSize % deviceAlignment) > 0 ? deviceAlignment : 0);
        layouts[i].bufferSize = uniformBufferSize * (layouts[i].instances == 0 ? 1 : layouts[i].instances) * layouts[i].dynamicAlignment;

        layouts[i].uniformBuffers.resize(swapChain.getImageCount());
        layouts[i].uniformBuffersMemory.resize(swapChain.getImageCount());

        for (size_t j = 0; j < swapChain.getImageCount(); j++) {
            gpu.createBuffer(
                layouts[i].bufferSize, 
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
                layouts[i].uniformBuffers[j], 
                layouts[i].uniformBuffersMemory[j]
            );
        }
    }
}

// void UniformBuffer::destroyUniformBuffers()
// {
//     for (size_t i = 0; i < dynamicUniformBuffers.size(); i++)
//     {        
//         vkDestroyBuffer(gpu.getDevice(), dynamicUniformBuffers[i], nullptr);
//         vkFreeMemory(gpu.getDevice(), dynamicUniformBuffersMemory[i], nullptr);
//         vkDestroyBuffer(gpu.getDevice(), staticUniformBuffers[i], nullptr);
//         vkFreeMemory(gpu.getDevice(), staticUniformBuffersMemory[i], nullptr);
//     }
// }

void UniformBuffer::_destroyUniformBuffers()
{
    for (size_t i = 0; i < layouts.size(); i++)
    {   
        for (size_t j = 0; j < layouts[i].uniformBuffers.size(); j++)
        {
            vkDestroyBuffer(gpu.getDevice(), layouts[i].uniformBuffers[j], nullptr);
            vkFreeMemory(gpu.getDevice(), layouts[i].uniformBuffersMemory[j], nullptr);
        }
    }
}

// void UniformBuffer::updateUniformBuffer(uint32_t imageIndex, DynamicUBO* ubo, StaticUBO& staticUbo)
// {
//     uint32_t size = dynamicAlignment * instances;

//     void* data;
//     vkMapMemory(gpu.getDevice(), dynamicUniformBuffersMemory[imageIndex], 0, size, 0, &data);
//         memcpy(data, ubo, size);
//     vkUnmapMemory(gpu.getDevice(), dynamicUniformBuffersMemory[imageIndex]);

//     void* staticUboData;
//     vkMapMemory(gpu.getDevice(), staticUniformBuffersMemory[imageIndex], 0, sizeof(staticUbo), 0, &staticUboData);
//         memcpy(staticUboData, &staticUbo, sizeof(staticUbo));
//     vkUnmapMemory(gpu.getDevice(), staticUniformBuffersMemory[imageIndex]);
// }

void UniformBuffer::updateUniformBuffer(uint32_t imageIndex, uint32_t i, uint32_t size, void* d)
{
    if (layouts[i].instances > 0)
    {        
        void* data;
        vkMapMemory(gpu.getDevice(), layouts[i].uniformBuffersMemory[imageIndex], 0, size, 0, &data);
            memcpy(data, d, size);
        vkUnmapMemory(gpu.getDevice(), layouts[i].uniformBuffersMemory[imageIndex]);
    }
    else
    {
        void* staticUboData;
        vkMapMemory(gpu.getDevice(), layouts[i].uniformBuffersMemory[imageIndex], 0, size, 0, &staticUboData);
            memcpy(staticUboData, d, size);
        vkUnmapMemory(gpu.getDevice(), layouts[i].uniformBuffersMemory[imageIndex]);
    }
}

UniformBuffer::UniformBuffer(
    GPU& _gpu, 
    SwapChain& _swapChain, 
    Pipeline& _pipeline, 
    std::vector<UniformLayout> _layouts
    // , uint32_t _instances
) : 
    gpu { _gpu }, 
    swapChain { _swapChain }, 
    pipeline { _pipeline }, 
    layouts { _layouts }
    // , instances { _instances }
{
    // createUniformBuffers();
    _createUniformBuffers();
    // createDescriptorPool();
    _createDescriptorPool();
    // createDescriptorSets();
    _createDescriptorSets();
}

UniformBuffer::~UniformBuffer()
{
    // destroyDescriptorSets();
    _destroyDescriptorSets();
    // destroyDescriptorPool();
    _destroyDescriptorPool();
    // destroyUniformBuffers();
    _destroyUniformBuffers();
}

const std::vector<VkDescriptorSet>& UniformBuffer::getDescriptorSets() const
{
    return descriptorSets;
}

// uint32_t UniformBuffer::getDynamicAlignment() const
// {
//     return dynamicAlignment;
// }

// uint32_t UniformBuffer::getBufferSize() const
// {
//     return bSize;
// }