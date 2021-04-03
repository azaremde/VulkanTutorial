#include "Image.hpp"

#include <stb_image/stb_image.h>

void Image::createTextureImage()
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) 
    {
        throw std::runtime_error("Failed to load texture image.");
    }

    gpu.createBuffer(
        imageSize, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        stagingBuffer, 
        stagingBufferMemory);

    void* data;
    vkMapMemory(gpu.getDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(gpu.getDevice(), stagingBufferMemory);

    stbi_image_free(pixels);

    gpu.createImage(
        texWidth, 
        texHeight, 
        VK_FORMAT_R8G8B8A8_SRGB, 
        VK_IMAGE_TILING_OPTIMAL, 
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        textureImage, 
        textureImageMemory);

    commandBuffer.transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    commandBuffer.copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    commandBuffer.transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    
    vkDestroyBuffer(gpu.getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(gpu.getDevice(), stagingBufferMemory, nullptr);
}

void Image::destroyTextureImage()
{
    vkDestroyImage(gpu.getDevice(), textureImage, nullptr);
    vkFreeMemory(gpu.getDevice(), textureImageMemory, nullptr);
}

void Image::createTextureImageView()
{    
    textureImageView = gpu.createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
}

void Image::destroyTextureImageView()
{
    vkDestroyImageView(gpu.getDevice(), textureImageView, nullptr);    
}

void Image::createTextureSampler()
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;

    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = gpu.props.limits.maxSamplerAnisotropy;
    
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
        vkCreateSampler(gpu.getDevice(), &samplerInfo, nullptr, &textureSampler), 
        "Failed to create texture sampler."
    );    
}

void Image::destroyTextureSampler()
{
    vkDestroySampler(gpu.getDevice(), textureSampler, nullptr);
}

Image::Image(GPU& _gpu, CommandBuffer& _commandBuffer, const std::string& _filename) : gpu { _gpu }, commandBuffer { _commandBuffer }, filename { _filename }
{
    createTextureImage();
    createTextureImageView();
    createTextureSampler();
}

Image::~Image()
{
    destroyTextureSampler();
    destroyTextureImageView();
    destroyTextureImage();
}