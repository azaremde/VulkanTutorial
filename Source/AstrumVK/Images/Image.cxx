#include "Image.hpp"

#include <stb_image/stb_image.h>

void Texture2D::createTextureImage(ImageAsset* imageAsset)
{
    // int texWidth, texHeight, texChannels;
    // stbi_uc* pixels = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);


    VkDeviceSize imageSize = imageAsset->texWidth * imageAsset->texHeight * 4;

    if (!imageAsset->pixels) 
    {
        throw std::runtime_error("Failed to load texture image.");
    }

    GPU::createBuffer(
        imageSize, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        stagingBuffer, 
        stagingBufferMemory);

    void* data;
    vkMapMemory(GPU::getDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, imageAsset->pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(GPU::getDevice(), stagingBufferMemory);

    GPU::createImage(
        imageAsset->texWidth, 
        imageAsset->texHeight, 
        VK_FORMAT_R8G8B8A8_SRGB, 
        VK_IMAGE_TILING_OPTIMAL, 
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        textureImage, 
        textureImageMemory);

    commandBuffer.transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    commandBuffer.copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(imageAsset->texWidth), static_cast<uint32_t>(imageAsset->texHeight));
    commandBuffer.transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    
    vkDestroyBuffer(GPU::getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(GPU::getDevice(), stagingBufferMemory, nullptr);
}

void Texture2D::destroyTextureImage()
{
    vkDestroyImage(GPU::getDevice(), textureImage, nullptr);
    vkFreeMemory(GPU::getDevice(), textureImageMemory, nullptr);
}

void Texture2D::createTextureImageView()
{    
    textureImageView = GPU::createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
}

void Texture2D::destroyTextureImageView()
{
    vkDestroyImageView(GPU::getDevice(), textureImageView, nullptr);    
}

void Texture2D::createTextureSampler()
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;

    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = GPU::props.limits.maxSamplerAnisotropy;
    
    // Todo: check IF!!!
    // samplerInfo.anisotropyEnable = VK_FALSE;
    // samplerInfo.maxAnisotropy = 1.0f;

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    VK_CHECK(
        vkCreateSampler(GPU::getDevice(), &samplerInfo, nullptr, &textureSampler), 
        "Failed to create texture sampler."
    );    
}

void Texture2D::destroyTextureSampler()
{
    vkDestroySampler(GPU::getDevice(), textureSampler, nullptr);
}

Texture2D::Texture2D(CommandBuffer& _commandBuffer, ImageAsset* imageAsset) : commandBuffer { _commandBuffer }
{
    createTextureImage(imageAsset);
    createTextureImageView();
    createTextureSampler();
}

Texture2D::~Texture2D()
{
    destroyTextureSampler();
    destroyTextureImageView();
    destroyTextureImage();
}