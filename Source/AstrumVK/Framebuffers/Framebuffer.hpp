#ifndef __AstrumVK_Framebuffers_Framebuffer_hpp__
#define __AstrumVK_Framebuffers_Framebuffer_hpp__

#pragma once

#include "Pch.hpp"

#include "AstrumVK/GPU/GPU.hpp"

class Framebuffer
{
private:
    VkFramebuffer framebuffer;

    GPU& gpu;

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

public:
    Framebuffer(GPU& _gpu, const VkExtent2D& size, const std::vector<VkImageView>& attachments, const VkRenderPass& renderPass);
    ~Framebuffer();
};

#endif