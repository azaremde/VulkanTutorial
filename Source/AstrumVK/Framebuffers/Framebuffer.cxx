#include "Framebuffer.hpp"

Framebuffer::Framebuffer(const VkExtent2D& size, const std::vector<VkImageView>& attachments, const VkRenderPass& renderPass)
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
        vkCreateFramebuffer(GPU::getDevice(), &framebufferInfo, nullptr, &framebuffer),
        "Failed to create framebuffer."
    );

    DebugLogOut("Framebuffer created.");
}

Framebuffer::~Framebuffer()
{
    vkDestroyFramebuffer(GPU::getDevice(), framebuffer, nullptr);
    DebugLogOut("Framebuffer destroyed.");
}

const VkFramebuffer& Framebuffer::getFramebuffer() const
{
    return framebuffer;
}