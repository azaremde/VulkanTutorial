#include "UniformBuffer.hpp"

void UniformBuffer::createDescriptorPool()
{
    std::vector<VkDescriptorPoolSize> poolSizes;

    for (size_t i = 0; i < layouts.size(); i++)
    {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = layouts[i].type;
        poolSize.descriptorCount = static_cast<uint32_t>(swapChain.getImageCount()) * amountOfEntities;
        poolSizes.emplace_back(poolSize);
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(swapChain.getImageCount()) * amountOfEntities;
    
    VK_CHECK(
        vkCreateDescriptorPool(GPU::getDevice(), &poolInfo, nullptr, &descriptorPool), 
        "Failed to create descriptor pool."
    );
}

void UniformBuffer::destroyDescriptorPool()
{    
    vkDestroyDescriptorPool(GPU::getDevice(), descriptorPool, nullptr);
}

void UniformBuffer::allocateDescriptorSets(uint32_t entityIndex)
{
    std::vector<VkDescriptorSetLayout> setLayouts(swapChain.getImageCount(), pipeline.getDescriptorSetLayout());
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChain.getImageCount());
    allocInfo.pSetLayouts = setLayouts.data();

    entities[entityIndex]->descriptorSets.resize(swapChain.getImageCount());

    VK_CHECK(
        vkAllocateDescriptorSets(GPU::getDevice(), &allocInfo, entities[entityIndex]->descriptorSets.data()), 
        "Failed to allocate descriptor sets."
    );

    for (size_t i = 0; i < swapChain.getImageCount(); i++) 
    {
        std::vector<VkWriteDescriptorSet> writeDescriptorSets;
        std::vector<VkDescriptorBufferInfo> bufferInfos;
        std::vector<VkDescriptorImageInfo> imageInfos;
        bufferInfos.resize(layouts.size());

        for (size_t j = 0; j < layouts.size(); j++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = layouts[j].uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = layouts[j].size;
            bufferInfos[j] = bufferInfo;

            if (layouts[j].type != VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
            {
                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = entities[entityIndex]->descriptorSets[i];
                descriptorWrite.dstBinding = layouts[j].binding;
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorType = layouts[j].type;
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.pBufferInfo = &bufferInfos[j];

                writeDescriptorSets.emplace_back(descriptorWrite);
            }
            else
            {
                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                imageInfo.imageView = layouts[j].imageViews[entityIndex];
                imageInfo.sampler = layouts[j].samplers[entityIndex];
                imageInfos.emplace_back(imageInfo);

                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = entities[entityIndex]->descriptorSets[i];
                descriptorWrite.dstBinding = layouts[j].binding;
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorType = layouts[j].type;
                descriptorWrite.descriptorCount = static_cast<uint32_t>(imageInfos.size());
                descriptorWrite.pImageInfo = imageInfos.data();

                writeDescriptorSets.emplace_back(descriptorWrite);
            }
        }

        vkUpdateDescriptorSets(
            GPU::getDevice(), 
            static_cast<uint32_t>(writeDescriptorSets.size()), 
            writeDescriptorSets.data(), 
            0, 
            nullptr
        );
    }
}

void UniformBuffer::createUniformBuffers()
{
    for (size_t i = 0; i < layouts.size(); i++)
    {
        uint32_t deviceAlignment = static_cast<uint32_t>(GPU::props.limits.minUniformBufferOffsetAlignment);
        uint32_t uniformBufferSize = layouts[i].size;    
        
        layouts[i].dynamicAlignment = (uniformBufferSize / deviceAlignment) * deviceAlignment + ((uniformBufferSize % deviceAlignment) > 0 ? deviceAlignment : 0);

        // Old variant:
        // layouts[i].bufferSize = (uniformBufferSize) * (layouts[i].instances == 0 ? 1 : layouts[i].instances) * layouts[i].dynamicAlignment;

        layouts[i].bufferSize = (uniformBufferSize + layouts[i].dynamicAlignment) * (layouts[i].instances == 0 ? 1 : layouts[i].instances);

        layouts[i].uniformBuffers.resize(swapChain.getImageCount());
        layouts[i].uniformBuffersMemory.resize(swapChain.getImageCount());

        for (size_t j = 0; j < swapChain.getImageCount(); j++) 
        {
            GPU::createBuffer(
                layouts[i].bufferSize, 
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                layouts[i].uniformBuffers[j], 
                layouts[i].uniformBuffersMemory[j]
            );
        }
    }
}

void UniformBuffer::destroyUniformBuffers()
{
    for (size_t i = 0; i < layouts.size(); i++)
    {   
        for (size_t j = 0; j < layouts[i].uniformBuffers.size(); j++)
        {
            vkDestroyBuffer(GPU::getDevice(), layouts[i].uniformBuffers[j], nullptr);
            vkFreeMemory(GPU::getDevice(), layouts[i].uniformBuffersMemory[j], nullptr);
        }
    }
}

void UniformBuffer::updateUniformBuffer(uint32_t imageIndex, uint32_t i, uint32_t size, void* d)
{    
    void* data;
    vkMapMemory(GPU::getDevice(), layouts[i].uniformBuffersMemory[imageIndex], 0, size, 0, &data);
        memcpy(data, d, size);
    vkUnmapMemory(GPU::getDevice(), layouts[i].uniformBuffersMemory[imageIndex]);
}

UniformBuffer::UniformBuffer(
    SwapChain& _swapChain, 
    Pipeline& _pipeline, 
    std::vector<UniformLayout> _layouts,
    std::vector<Entity*>& _entities
) :
    swapChain { _swapChain }, 
    pipeline { _pipeline }, 
    layouts { _layouts },
    amountOfEntities { static_cast<uint32_t>(_entities.size()) },
    entities { _entities }
{
    createUniformBuffers();
    createDescriptorPool();
    
    for (uint32_t entityIndex = 0; entityIndex < entities.size(); entityIndex++)
    {
        allocateDescriptorSets(entityIndex);
    }
}

UniformBuffer::~UniformBuffer()
{
    destroyDescriptorPool();
    destroyUniformBuffers();
}