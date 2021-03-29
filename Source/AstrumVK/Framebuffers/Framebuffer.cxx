#include "Framebuffer.hpp"

Framebuffer::Framebuffer(GPU& _gpu, const VkExtent2D& size, const std::vector<VkImageView>& attachments, const VkRenderPass& renderPass) : gpu { _gpu }
{    
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = size.width;
    framebufferInfo.height = size.height;
    framebufferInfo.layers = 1;

    VK_CHECK(
        vkCreateFramebuffer(gpu.getDevice(), &framebufferInfo, nullptr, &framebuffer),
        "Failed to create framebuffer."
    );

    DebugLogOut("Framebuffer created.");
}

Framebuffer::~Framebuffer()
{
    vkDestroyFramebuffer(gpu.getDevice(), framebuffer, nullptr);
    DebugLogOut("Framebuffer destroyed.");
}

const VkFramebuffer& Framebuffer::getFramebuffer() const
{
    return framebuffer;
}