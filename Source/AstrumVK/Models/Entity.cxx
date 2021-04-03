#include "Entity.hpp"

#include "AstrumVK/Images/Image.hpp"

void Entity::destroy(GPU& gpu)
{    
    vkDestroyBuffer(gpu.getDevice(), buffer, nullptr);
    vkFreeMemory(gpu.getDevice(), memory, nullptr);
    vkDestroyBuffer(gpu.getDevice(), indexBuffer, nullptr);
    vkFreeMemory(gpu.getDevice(), indexBufferMemory, nullptr);

    delete albedoColor;
}

Entity::Entity()
{

}

Entity::~Entity()
{

}