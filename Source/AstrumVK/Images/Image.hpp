#ifndef __AstrumVK_Images_Image_hpp__
#define __AstrumVK_Images_Image_hpp__

#pragma once

#include "Pch.hpp"

#include "AstrumVK/GPU/GPU.hpp"
#include "AstrumVK/GPU/CommandBuffer.hpp"
#include "Assets/ImageAsset.hpp"

class Texture2D
{
private:
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    void createTextureImage(ImageAsset* imageAsset);
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

    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;

public:
    VkImageView getImageView()
    {
        return textureImageView;
    }

    VkSampler getSampler()
    {
        return textureSampler;
    }

    Texture2D(GPU& _gpu, CommandBuffer& _commandBuffer, ImageAsset* imageAsset);
    ~Texture2D();
};

#endif