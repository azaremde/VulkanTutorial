#include "VAO.hpp"

void VAO::destroy(GPU& gpu)
{    
    vkDestroyBuffer(gpu.getDevice(), buffer, nullptr);
    vkFreeMemory(gpu.getDevice(), memory, nullptr);
}

VAO::VAO()
{

}

VAO::~VAO()
{

}