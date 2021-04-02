#ifndef __AstrumVK_Images_Image_hpp__
#define __AstrumVK_Images_Image_hpp__

#pragma once

#include "Pch.hpp"

#include "AstrumVK/GPU/GPU.hpp"
#include "AstrumVK/GPU/CommandBuffer.hpp"

class Image
{
private:
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    VkImage textureImage;
    VkDeviceMemory textureImageMemory;

    void createTextureImage();
    void destroyTextureImage();
    
    GPU& gpu;
    CommandBuffer& commandBuffer;

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

public:
    Image(GPU& _gpu, CommandBuffer& _commandBuffer);
    ~Image();
};

#endif