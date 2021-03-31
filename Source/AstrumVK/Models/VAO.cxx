#include "VAO.hpp"

void VAO::destroy(GPU& gpu)
{    
    vkDestroyBuffer(gpu.getDevice(), buffer, nullptr);
    vkFreeMemory(gpu.getDevice(), memory, nullptr);
    vkDestroyBuffer(gpu.getDevice(), indexBuffer, nullptr);
    vkFreeMemory(gpu.getDevice(), indexBufferMemory, nullptr);
}

VAO::VAO()
{

}

VAO::~VAO()
{

}