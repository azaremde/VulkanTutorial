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

    VkImageView textureImageView;
    void createTextureImageView();
    void destroyTextureImageView();

    VkSampler textureSampler;
    void createTextureSampler();
    void destroyTextureSampler();

    std::string filename;
    
    GPU& gpu;
    CommandBuffer& commandBuffer;

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

public:
    VkImageView getImageView()
    {
        return textureImageView;
    }

    VkSampler getSampler()
    {
        return textureSampler;
    }

    Image(GPU& _gpu, CommandBuffer& _commandBuffer, const std::string& _filename);
    ~Image();
};

#endif